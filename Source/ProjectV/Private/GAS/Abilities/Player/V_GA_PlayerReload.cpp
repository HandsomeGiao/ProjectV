// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Player/V_GA_PlayerReload.h"

#include "VGameplayTags.h"
#include "GAS/ASC/V_WeaponASC.h"
#include "Pickups/Weapons/V_WeaponBase.h"
#include "Player/V_PlayerCharacter.h"

void UV_GA_PlayerReload::WeaponReload()
{
    // 和PrimaryAttack类似，这个能力只会在Server上激活（因为所有权问题，玩家不能随意激活Weapon的GA）
    if (!PlayerChar.IsValid())
        PlayerChar = CastChecked<AV_PlayerCharacter>(GetAvatarActorFromActorInfo());
    AV_WeaponBase* Weapon = Cast<AV_WeaponBase>(PlayerChar->GetCurrentWeapon());
    // tips ： 激活这个能力时，要求玩家一定要持有有效的武器
    checkf(IsValid(Weapon), TEXT("WeaponReload: CurrentWeapon is null in %s"), *GetName());

    UV_WeaponASC* WeaponASC = CastChecked<UV_WeaponASC>(Weapon->GetAbilitySystemComponent());
    FGameplayTagContainer ReloadTag;
    ReloadTag.AddTag(VTags::VAbilites::VWeapons::WeaponReload);
    WeaponASC->TryActivateAbilitiesByTag(ReloadTag);
}