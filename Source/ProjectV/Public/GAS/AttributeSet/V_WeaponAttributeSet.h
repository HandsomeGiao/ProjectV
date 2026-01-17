// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "V_WeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class PROJECTV_API UV_WeaponAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

protected:
    // ======= override func =======
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
    // =============== Attribute ===========
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ammo)
    FGameplayAttributeData CurrentAmmo;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmo)
    FGameplayAttributeData MaxAmmo;

    // ============= 内部辅助方法 =============
    UFUNCTION()
    void OnRep_Ammo(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MaxAmmo(const FGameplayAttributeData& OldValue);

    ATTRIBUTE_ACCESSORS(UV_WeaponAttributeSet, CurrentAmmo);
    ATTRIBUTE_ACCESSORS(UV_WeaponAttributeSet, MaxAmmo);
};