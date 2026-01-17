// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/V_PlayerAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UV_PlayerAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PistolAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RifleAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ShotgunAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxPistolAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxRifleAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxShotgunAmmo, COND_None, REPNOTIFY_Always);
}

void UV_PlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
}

void UV_PlayerAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // Clamp Health to [0, MaxHealth]
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetPistolAmmoAttribute())
    {
        SetPistolAmmo(FMath::Clamp(GetPistolAmmo(), 0.f, GetMaxPistolAmmo()));
    }
    else if (Data.EvaluatedData.Attribute == GetRifleAmmoAttribute())
    {
        SetRifleAmmo(FMath::Clamp(GetRifleAmmo(), 0.f, GetMaxRifleAmmo()));
    }
    else if (Data.EvaluatedData.Attribute == GetShotgunAmmoAttribute())
    {
        SetShotgunAmmo(FMath::Clamp(GetShotgunAmmo(), 0.f, GetMaxShotgunAmmo()));
    }
}

// Replication Function
void UV_PlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UV_PlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UV_PlayerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}

void UV_PlayerAttributeSet::OnRep_PistolAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PistolAmmo, OldValue);
}

void UV_PlayerAttributeSet::OnRep_RifleAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, RifleAmmo, OldValue);
}

void UV_PlayerAttributeSet::OnRep_ShotgunAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ShotgunAmmo, OldValue);
}

void UV_PlayerAttributeSet::OnRep_MaxPistolAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxPistolAmmo, OldValue);
}

void UV_PlayerAttributeSet::OnRep_MaxRifleAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxRifleAmmo, OldValue);
}

void UV_PlayerAttributeSet::OnRep_MaxShotgunAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxShotgunAmmo, OldValue);
}

// ~Replication Function