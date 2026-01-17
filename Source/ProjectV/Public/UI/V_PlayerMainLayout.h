// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "V_PlayerMainLayout.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTV_API UV_PlayerMainLayout final: public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void SetAmmo(float CurrentAmmo);
    UFUNCTION(BlueprintImplementableEvent)
    void SetMaxAmmo(float MaxAmmo);
    UFUNCTION(BlueprintImplementableEvent)
    void SetHealth(float NewHealth);
    UFUNCTION(BlueprintImplementableEvent)
    void SetMaxHealth(float NewMaxHealth);
    UFUNCTION(BlueprintImplementableEvent)
    void SetWeapon0Icon(UTexture2D* Icon);
    UFUNCTION(BlueprintImplementableEvent)
    void SetWeapon1Icon(UTexture2D* Icon);
};