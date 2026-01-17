// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "V_GA_WeaponFire.generated.h"

class AV_Bullet;
/**
 * 
 */
UCLASS()
class PROJECTV_API UV_GA_WeaponFire : public UGameplayAbility
{
    GENERATED_BODY()

protected:
    // ======== 配置变量 ========
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    FGameplayTag FireGCTag;
    
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TSubclassOf<AV_Bullet> BulletClass;
    
    UFUNCTION(BlueprintCallable)
    void SpawnFx(int32 NumBullets = 1);
};