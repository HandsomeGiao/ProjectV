// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Weapons/V_GA_WeaponReload.h"

#include "VGameplayTags.h"
#include "GAS/ASC/V_PlayerASC.h"
#include "GAS/ASC/V_WeaponASC.h"
#include "GAS/AttributeSet/V_PlayerAttributeSet.h"
#include "GAS/AttributeSet/V_WeaponAttributeSet.h"
#include "Pickups/Weapons/V_WeaponBase.h"
#include "Player/V_PlayerCharacter.h"
#include "ProjectV/ProjectV.h"

void UV_GA_WeaponReload::SwitchAmmo()
{
    AV_WeaponBase* Weapon = CastChecked<AV_WeaponBase>(GetAvatarActorFromActorInfo());
    // only called on server
    if (!Weapon->HasAuthority())
    {
        UE_LOG(LogProjectV, Error, TEXT("SwitchAmmo called on client in %s"), *GetName());
        return;
    }
    UV_WeaponASC* WeaponASC = CastChecked<UV_WeaponASC>(Weapon->GetAbilitySystemComponent());
    if (!IsValid(Weapon->GetOwningPlayer()))
    {
        UE_LOG(LogProjectV, Error, TEXT("SwitchAmmo: Weapon has no OwningPlayer in %s"), *GetName());
        return;
    }
    UV_PlayerASC* PlayerASC = CastChecked<UV_PlayerASC>(Weapon->GetOwningPlayer()->GetAbilitySystemComponent());

    float ReserveAmmo;
    switch (Weapon->GetWeaponType())
    {
        case EWeaponType::Pistol:
            ReserveAmmo = PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetPistolAmmoAttribute());
            break;
        case EWeaponType::Rifle:
            ReserveAmmo = PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetRifleAmmoAttribute());
            break;
        case EWeaponType::Shotgun:
            ReserveAmmo = PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetShotgunAmmoAttribute());
            break;
        default:
            UE_LOG(LogProjectV, Error, TEXT("Unknown weapon type in SwitchAmmo in %s"), *GetName());
            return;
    }

    const float MaxAmmo = WeaponASC->GetNumericAttribute(UV_WeaponAttributeSet::GetMaxAmmoAttribute());
    const float CurrentAmmo = WeaponASC->GetNumericAttribute(UV_WeaponAttributeSet::GetCurrentAmmoAttribute());
    const float AmmoSwitched = FMath::Max(FMath::Min(MaxAmmo - CurrentAmmo, ReserveAmmo), 0.f);

    if (AmmoSwitched > 0.f)
    {
        // 扣除玩家的备用弹药
        FGameplayEffectContextHandle Context = PlayerASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = PlayerASC->MakeOutgoingSpec(PlayerConsumeAmmoEffectClass, 1.0f, Context);
        check(SpecHandle.IsValid());
        switch (Weapon->GetWeaponType())
        {
            case EWeaponType::Pistol:
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumePisotlAmmo, -AmmoSwitched);
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumeRifleAmmo, 0.f);
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumeShotgunAmmo, 0.f);
                break;
            case EWeaponType::Rifle:
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumeRifleAmmo, -AmmoSwitched);
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumePisotlAmmo, 0.f);
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumeShotgunAmmo, 0.f);
                break;
            case EWeaponType::Shotgun:
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumePisotlAmmo, 0.f);
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumeRifleAmmo, 0.f);
                SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::PlayerConsumeShotgunAmmo, -AmmoSwitched);
                break;
            default:
                // won't reach here
                checkf(false, TEXT("Unknown weapon type in SwitchAmmo in %s"), *GetName());
        }
        PlayerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        // 添加武器弹药
        Context = WeaponASC->MakeEffectContext();
        SpecHandle = WeaponASC->MakeOutgoingSpec(WeaponAddAmmoEffectClass, 1.0f, Context);
        check(SpecHandle.IsValid());
        SpecHandle.Data->SetSetByCallerMagnitude(VTags::VGEs::WeaponAddAmmo, AmmoSwitched);
        WeaponASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
    else
    {
        UE_LOG(LogProjectV, Log, TEXT("No ammo to switch in SwitchAmmo in %s"), *GetName());
    }
}