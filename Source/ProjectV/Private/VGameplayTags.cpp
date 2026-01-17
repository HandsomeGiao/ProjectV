#include "VGameplayTags.h"

namespace VTags
{
    namespace VAbilites
    {
        UE_DEFINE_GAMEPLAY_TAG(ActivateOnGiven, "VTags.Abilities.ActivateOnGiven");
        UE_DEFINE_GAMEPLAY_TAG(DebugGA, "VTags.Abilities.DebugGA");

        namespace VWeapons
        {
            UE_DEFINE_GAMEPLAY_TAG(WeaponPrimaryAttack, "VTags.Abilities.Weapons.PrimaryAttack");
            UE_DEFINE_GAMEPLAY_TAG(WeaponReload, "VTags.Abilities.Weapons.Reload");
            UE_DEFINE_GAMEPLAY_TAG(WeaponReloadComplete, "VTags.Abilities.Weapons.ReloadComplete");
        }

        namespace Player
        {
            UE_DEFINE_GAMEPLAY_TAG(PlayerPrimaryAttack, "VTags.Abilities.Player.PrimaryAttack");
            UE_DEFINE_GAMEPLAY_TAG(PlayerReload, "VTags.Abilities.Player.Reload");
            UE_DEFINE_GAMEPLAY_TAG(PlayerPrimaryAttackEnd, "VTags.Abilities.Player.PrimaryAttackEnd");
            UE_DEFINE_GAMEPLAY_TAG(PlayerThrowThrowable, "VTags.Abilities.Player.ThrowThrowable");
        }
    }

    namespace Status
    {
        UE_DEFINE_GAMEPLAY_TAG(PlayerEquippedWeapon, "VTags.Status.Player.EquippedWeapon");

        namespace Player
        {
            UE_DEFINE_GAMEPLAY_TAG(IsThrowingThrowable, "VTags.Status.Player.IsThrowingThrowable");
            UE_DEFINE_GAMEPLAY_TAG(GotFlashed, "VTags.Status.Player.GotFlashed");
        }
    }

    namespace VGEs
    {
        UE_DEFINE_GAMEPLAY_TAG(WeaponAddAmmo, "VTags.GEs.Weapon.AddAmmo");      
        UE_DEFINE_GAMEPLAY_TAG(PlayerConsumePisotlAmmo, "VTags.GEs.Player.ConsumePistolAmmo");
        UE_DEFINE_GAMEPLAY_TAG(PlayerConsumeRifleAmmo, "VTags.GEs.Player.ConsumeRifleAmmo");
        UE_DEFINE_GAMEPLAY_TAG(PlayerConsumeShotgunAmmo, "VTags.GEs.Player.ConsumeShotgunAmmo");
        UE_DEFINE_GAMEPLAY_TAG(ExplosionDamage, "VTags.GEs.ExplosionDamage");

        namespace Weapon
        {
            UE_DEFINE_GAMEPLAY_TAG(RifleFireCD, "VTags.GEs.Weapon.RifleFireCD");
        }
    }

    namespace VCues
    {
        UE_DEFINE_GAMEPLAY_TAG(FireFX_Rifle, "GameplayCue.VTags.FireFX.Rifle");
        UE_DEFINE_GAMEPLAY_TAG(FireFX_Pistol, "GameplayCue.VTags.FireFX.Pistol");
        UE_DEFINE_GAMEPLAY_TAG(FireFX_Shotgun, "GameplayCue.VTags.FireFX.Shotgun");
    }

    namespace Events
    {
        namespace Player
        {
            UE_DEFINE_GAMEPLAY_TAG(PlayerConfirmThrowThrowable, "VTags.Events.Player.ConfirmThrowThrowable");
        }
    }
}