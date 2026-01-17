// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "V_GA_PrimaryAttack.generated.h"

class AV_PlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTV_API UV_GA_PrimaryAttack : public UGameplayAbility
{
    GENERATED_BODY()

protected:
    UFUNCTION(BlueprintCallable)
    void ActivateWeaponPrimaryAttack();

private:
    TWeakObjectPtr<AV_PlayerCharacter> PlayerChar;
};