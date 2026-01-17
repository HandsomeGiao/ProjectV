// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/V_WeaponAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UV_WeaponAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentAmmo, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxAmmo, COND_None, REPNOTIFY_Always);
}

void UV_WeaponAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetCurrentAmmoAttribute())
    {
        SetCurrentAmmo(FMath::Clamp(GetCurrentAmmo(), 0.f, GetMaxAmmo()));
    }
}

void UV_WeaponAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CurrentAmmo, OldValue);
}

void UV_WeaponAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxAmmo, OldValue);
}