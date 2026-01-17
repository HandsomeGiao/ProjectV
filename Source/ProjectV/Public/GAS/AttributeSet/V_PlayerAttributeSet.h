// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "V_PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PROJECTV_API UV_PlayerAttributeSet final : public UAttributeSet
{
    GENERATED_BODY()

protected:
    // ======= override func =======
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
    // =============== Attribute ===========

    // 生命值
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    // 备弹数量
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PistolAmmo)
    FGameplayAttributeData PistolAmmo;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_RifleAmmo)
    FGameplayAttributeData RifleAmmo;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ShotgunAmmo)
    FGameplayAttributeData ShotgunAmmo;
    // 这里的MaxAmmo指的最大携弹量，Ammo才是应该显示在屏幕中的备弹数量
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxPistolAmmo)
    FGameplayAttributeData MaxPistolAmmo;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxRifleAmmo)
    FGameplayAttributeData MaxRifleAmmo;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxShotgunAmmo)
    FGameplayAttributeData MaxShotgunAmmo;

    // ============ 内部辅助方法 =============
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_PistolAmmo(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_RifleAmmo(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_ShotgunAmmo(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MaxPistolAmmo(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MaxRifleAmmo(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MaxShotgunAmmo(const FGameplayAttributeData& OldValue);

    ATTRIBUTE_ACCESSORS(ThisClass, Health);
    ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
    ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeed);
    ATTRIBUTE_ACCESSORS(ThisClass, PistolAmmo);
    ATTRIBUTE_ACCESSORS(ThisClass, RifleAmmo);
    ATTRIBUTE_ACCESSORS(ThisClass, ShotgunAmmo);
    ATTRIBUTE_ACCESSORS(ThisClass, MaxPistolAmmo);
    ATTRIBUTE_ACCESSORS(ThisClass, MaxRifleAmmo);
    ATTRIBUTE_ACCESSORS(ThisClass, MaxShotgunAmmo);
};