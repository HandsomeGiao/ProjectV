#include "Miscellaneous/ThrowableInstance/V_Instance_Explosive.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "VGameplayTags.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectV/ProjectV.h"

AV_Instance_Explosive::AV_Instance_Explosive()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AV_Instance_Explosive::BeginPlay()
{
    Super::BeginPlay();
    // GetWorld在此一定有效
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(
                DestroyTimerHandle, this, &ThisClass::OnTimerExpired, 5.f, false);
    }
}

void AV_Instance_Explosive::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogProjectV, Warning, TEXT("EndPlay: Invalid World in %s"), *GetName());
        return;
    }
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
                World,
                ExplosionEffect,
                GetActorLocation(),
                FRotator::ZeroRotator,
                FVector(1.f),
                true);
    }

    if (HasAuthority() && ExplosionEffect)
    {
        if (IsValid(GetWorld()) && GetWorldTimerManager().IsTimerActive(DestroyTimerHandle))
        {
            // clear timer to avoid multiple or invalid calls
            GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
        }

        // 找到范围内的角色并通过ExplosionDamageEffectClass造成伤害
        // 通过设置SetByCaller传递伤害数值
        if (!HasAuthority() || !IsValid(ExplosionDamageEffectClass) || ExplosionDamage <= 0.f)
        {
            return;
        }

        TArray<FOverlapResult> Overlaps;
        const FVector Origin = GetActorLocation();
        const FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);
        FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ThrowableExplosionOverlap), false, this);
        FCollisionObjectQueryParams ObjectQueryParams;
        ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
        ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

        if (!World->OverlapMultiByObjectType(
                Overlaps, Origin, FQuat::Identity, ObjectQueryParams, Sphere, QueryParams))
        {
            return;
        }

        TSet<AActor*> ProcessedActors;
        for (const FOverlapResult& Overlap : Overlaps)
        {
            AActor* OtherActor = Overlap.GetActor();
            if (!IsValid(OtherActor) || OtherActor == this || ProcessedActors.Contains(OtherActor))
            {
                continue;
            }
            ProcessedActors.Add(OtherActor);

            IAbilitySystemInterface* ASInterface = Cast<IAbilitySystemInterface>(OtherActor);
            if (!ASInterface)
            {
                continue;
            }

            UAbilitySystemComponent* TargetASC = ASInterface->GetAbilitySystemComponent();
            if (!IsValid(TargetASC))
            {
                continue;
            }

            FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
            EffectContext.AddSourceObject(this);
            FGameplayEffectSpecHandle SpecHandle =
                    TargetASC->MakeOutgoingSpec(ExplosionDamageEffectClass, 1.f, EffectContext);
            if (!SpecHandle.IsValid())
            {
                continue;
            }

            SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::ExplosionDamage, -ExplosionDamage);
            TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}

void AV_Instance_Explosive::OnTimerExpired()
{
    Destroy();
}