#include "Miscellaneous/V_Bullet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

void AV_Bullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
                                OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //UE_LOG(LogProjectV, Log, TEXT("Bullet %s overlapped with %s"), *GetName(), *OtherActor->GetName());

    // 需要注意，虽然这个Actor是复制的，但是为了客户端的表现，子弹的碰撞检测仍然在客户端进行
    // 因此可能客户端上会Destroy该Actor，但扣血逻辑只会在服务器上进行。
    Destroy();

    // 播放声音与特效
    if (IsValid(BulletHitSound))
    {
        UGameplayStatics::PlaySoundAtLocation(this, BulletHitSound, SweepResult.ImpactPoint);
    }
    if (IsValid(BulletHitParticle))
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticle, SweepResult.ImpactPoint,
                                                 SweepResult.ImpactNormal.Rotation());
    }

    if (HasAuthority())
    {
        IAbilitySystemInterface* ASInterface = Cast<IAbilitySystemInterface>(OtherActor);
        if (ASInterface && IsValid(DamageEffectClass))
        {
            if (UAbilitySystemComponent* TargetASC = ASInterface->GetAbilitySystemComponent())
            {
                FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
                FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1, EffectContext);
                if (SpecHandle.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                    //UE_LOG(LogProjectV, Log, TEXT("Applied Damage Effect to %s from Bullet %s"), *OtherActor->GetName(),
                    // *GetName());
                }
            }
        }
    }
}

AV_Bullet::AV_Bullet()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    SetRootComponent(CollisionComponent);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBulletOverlap);

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->Bounciness = 1.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AV_Bullet::OnDestroyTimerExpired()
{
    Destroy();
}

void AV_Bullet::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::OnDestroyTimerExpired, 5.f, false);
}

void AV_Bullet::Destroyed()
{
    Super::Destroyed();
    GetWorld()->GetTimerManager().ClearTimer(DestroyTimerHandle);
}