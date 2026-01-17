// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "V_GA_WeaponReload.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTV_API UV_GA_WeaponReload : public UGameplayAbility
{
    GENERATED_BODY()

protected:
    // ========= 内部方法 =======
    UFUNCTION(BlueprintCallable)
    void SwitchAmmo();

    // ========= 配置变量 ========
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TSubclassOf<UGameplayEffect> PlayerConsumeAmmoEffectClass;
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TSubclassOf<UGameplayEffect> WeaponAddAmmoEffectClass;
};