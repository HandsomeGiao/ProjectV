#include "Miscellaneous/ThrowableInstance/V_FireArea.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectV/ProjectV.h"

void AV_FireArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
    UE_LOG(LogProjectV, Log, TEXT("FireArea overlapped with %s"), *OtherActor->GetName());
    // 给Actor施加火焰伤害效果
    if (IsValid(FireDamageGE) && IsValid(OtherActor))
    {
        IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
        if (!ASI)
            return;
        UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
        if (IsValid(ASC))
        {
            FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(FireDamageGE, 1.0f, Context);
            check(SpecHandle.IsValid());
            ActiveEffectMap.Add(ASC, ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()));
        }
    }
}

void AV_FireArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // DO nothing for now
    if (IsValid(OtherActor))
    {
        IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
        if (!ASI)
            return;
        UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
        if (IsValid(ASC) && ActiveEffectMap.Contains(ASC))
        {
            ASC->RemoveActiveGameplayEffect(ActiveEffectMap[ASC]);
            ActiveEffectMap.Remove(ASC);
            UE_LOG(LogProjectV, Log, TEXT("FireArea removed effect from %s"), *OtherActor->GetName());
        }
    }
}

void AV_FireArea::TimerExpired()
{
    Destroy();
}

void AV_FireArea::RemoveAllEffects()
{
    for (auto& Elem : ActiveEffectMap)
    {
        UAbilitySystemComponent* ASC = Elem.Key;
        if (IsValid(ASC))
        {
            ASC->RemoveActiveGameplayEffect(Elem.Value);
        }
    }
    ActiveEffectMap.Empty();
}

AV_FireArea::AV_FireArea()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    SetRootComponent(BoxComp);
    if (HasAuthority())
    {
        BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AV_FireArea::OnOverlapBegin);
        BoxComp->OnComponentEndOverlap.AddDynamic(this, &AV_FireArea::OnOverlapEnd);
    }

    FireArc = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireArc"));
    FireArc->SetupAttachment(GetRootComponent());
}

void AV_FireArea::BeginPlay()
{
    Super::BeginPlay();
    FireArc->Activate();

    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(TimerHandle_LifeSpan, this, &AV_FireArea::TimerExpired, FireLifeSpan, false);
    }
}

void AV_FireArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(TimerHandle_LifeSpan);
    RemoveAllEffects();
}