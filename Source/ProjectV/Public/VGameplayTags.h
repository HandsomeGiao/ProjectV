#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace VTags
{
    namespace VAbilites
    {
        // Activate ability when given
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(DebugGA);

        namespace VWeapons
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponPrimaryAttack);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponReload);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponReloadComplete);
        }

        namespace Player
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerPrimaryAttack);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerReload);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerPrimaryAttackEnd);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerThrowThrowable);
        }
    }

    namespace Status
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerEquippedWeapon);

        namespace Player
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(IsThrowingThrowable);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(GotFlashed);
        }
    }

    namespace VGEs
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponAddAmmo);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerConsumePisotlAmmo);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerConsumeRifleAmmo);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerConsumeShotgunAmmo);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExplosionDamage);

        namespace Weapon
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(RifleFireCD);
        }
    }

    namespace VCues
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireFX_Rifle);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireFX_Pistol);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireFX_Shotgun);
    }

    namespace Events
    {
        namespace Player
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerConfirmThrowThrowable);
        }
    }
}