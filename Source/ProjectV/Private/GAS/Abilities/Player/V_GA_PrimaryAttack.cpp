// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Player/V_GA_PrimaryAttack.h"

#include "AbilitySystemComponent.h"
#include "VGameplayTags.h"
#include "Pickups/Weapons/V_WeaponBase.h"
#include "Player/V_PlayerCharacter.h"
#include "ProjectV/ProjectV.h"

// 激活这个能力时，要求玩家一定要持有有效的武器
void UV_GA_PrimaryAttack::ActivateWeaponPrimaryAttack()
{
    if (!PlayerChar.IsValid())
        PlayerChar = Cast<AV_PlayerCharacter>(GetAvatarActorFromActorInfo());

    checkf(PlayerChar.IsValid(), TEXT("ActivateWeaponPrimaryAttack: PlayerChar is null in %s"), *GetName());
    if (!IsValid(PlayerChar->GetCurrentWeapon()))
    {
        UE_LOG(LogProjectV, Warning,
               TEXT("ActivateWeaponPrimaryAttack: PlayerChar has no CurrentWeapon in %s"), *GetName());
        return;
    }

    UAbilitySystemComponent* WeaponASC = PlayerChar->GetCurrentWeapon()->GetAbilitySystemComponent();
    checkf(IsValid(WeaponASC), TEXT("ActivateWeaponPrimaryAttack: WeaponASC is null in %s"), *GetName());
    FGameplayTagContainer PrimaryAttackTag;
    PrimaryAttackTag.AddTag(VTags::VAbilites::VWeapons::WeaponPrimaryAttack);
    WeaponASC->TryActivateAbilitiesByTag(PrimaryAttackTag);
}