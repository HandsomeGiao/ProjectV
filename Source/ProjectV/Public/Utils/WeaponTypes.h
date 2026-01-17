#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None UMETA(DisplayName = "None"),
    Pistol UMETA(DisplayName = "Pistol"),
    Rifle UMETA(DisplayName = "Rifle"),
    Shotgun UMETA(DisplayName = "Shotgun"),
};