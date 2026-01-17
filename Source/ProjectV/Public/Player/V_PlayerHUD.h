// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/HUD.h"
#include "Pickups/Weapons/V_WeaponBase.h"
#include "V_PlayerHUD.generated.h"

class AV_PlayerCharacter;
class UAbilitySystemComponent;
class UV_PlayerMainLayout;

UCLASS()
class PROJECTV_API AV_PlayerHUD final : public AHUD
{
    GENERATED_BODY()

public:

    // ====== 修改接口 =======
    void InitLayout(AV_PlayerCharacter* InPlayerChar);
    void SetWeapon(AV_WeaponBase* InWeapon);
    void SetHealth(float NewHealth);
    void SetMaxHealth(float NewMaxHealth);
    void SetCurrentAmmo(float CurrentAmmo);
    void SetReserveAmmo(float MaxAmmo);
    void SetWeapon0Icon(UTexture2D* Icon);
    void SetWeapon1Icon(UTexture2D* Icon);

protected:
    
    // ====== override func =======
    virtual void Destroyed() override;
    
private:
    
    // ======== 辅助方法 ======
    void RemoveInvalidDelegate();

    // ======= 配置变量 =======
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UV_PlayerMainLayout> PlayerMainLayoutClass;
    
    // ======= 内部变量 =======
    
    UPROPERTY()
    TObjectPtr<UV_PlayerMainLayout> PlayerMainLayout;
    
    TWeakObjectPtr<AV_PlayerCharacter> PlayerChar;
    TWeakObjectPtr<UAbilitySystemComponent> WeaponASC;

    // 当前绑定的delegate
    FOnGameplayAttributeValueChange* ReserverAmmoChangeDelegate{nullptr};
    // health , max health, reserve ammo(player's current ammo)
    TStaticArray<FDelegateHandle, 3> PlayerASCDelegateHandles;
    // weapon ammo
    TStaticArray<FDelegateHandle, 1> WeaponASCDelegateHandles;
};