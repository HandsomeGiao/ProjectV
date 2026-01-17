// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "V_ThrowThrowable.generated.h"

class AV_ThrowableInstance;
/**
 * 
 */
UCLASS()
class PROJECTV_API UV_ThrowThrowable : public UGameplayAbility
{
    GENERATED_BODY()

protected:
    UFUNCTION(BlueprintCallable)
    void SpawnThrowable(TSubclassOf<AV_ThrowableInstance> ThrowableClass, FVector2D AimDirection, float ThrowSpeed);
};