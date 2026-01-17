// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "V_PlayerASC.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTV_API UV_PlayerASC final : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UV_PlayerASC();

protected:
    // ======== override func ========
    virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};