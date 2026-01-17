#include "Base/V_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "ProjectV/ProjectV.h"

AV_BaseCharacter::AV_BaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AV_BaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AV_BaseCharacter::GiveStartupAbilities()
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("Tried to give abilities on client in %s"), *GetName());
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!IsValid(ASC))
    {
        UE_LOG(LogProjectV, Warning, TEXT("AbilitySystemComponent is not valid in %s"), *GetName());
        return;
    }

    for (const auto& Ability : StartupAbilities)
    {
        ASC->GiveAbility(Ability);
    }
}

void AV_BaseCharacter::InitializeAttributes()
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("Tried to initialize attributes on client in %s"), *GetName());
        return;
    }

    if (!IsValid(InitializeAttributesEffect))
    {
        UE_LOG(LogProjectV, Warning, TEXT("InitializeAttributesEffect is not set in %s"), *GetName());
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!IsValid(ASC))
    {
        UE_LOG(LogProjectV, Warning, TEXT("AbilitySystemComponent is not valid in %s"), *GetName());
        return;
    }

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InitializeAttributesEffect, 1, EffectContext);

    if (SpecHandle.IsValid())
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}