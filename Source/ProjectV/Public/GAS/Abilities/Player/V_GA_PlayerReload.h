// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "V_GA_PlayerReload.generated.h"

class AV_PlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTV_API UV_GA_PlayerReload : public UGameplayAbility
{
    GENERATED_BODY()

protected:
    UFUNCTION(BlueprintCallable)
    void WeaponReload();
    
    TWeakObjectPtr<AV_PlayerCharacter> PlayerChar;
};