// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "Pickups/V_PickupsBase.h"
#include "Utils/WeaponTypes.h"
#include "V_WeaponBase.generated.h"

class AV_PlayerCharacter;
class UGameplayEffect;
class UV_WeaponAttributeSet;
class UV_WeaponASC;
class UGameplayAbility;

UCLASS()
class PROJECTV_API AV_WeaponBase : public AV_PickupsBase, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AV_WeaponBase();

    // ======= 修改接口 ======
    virtual void Thrownup() override;
    void Unequipped();
    void Equipped();
    virtual void PickedUpBy(AActor* InActor) override;

    UFUNCTION(BlueprintCallable)
    void AddSpreadAngle();

    // ======= 查询接口 ======
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    float GetSpreadAngle() const { return SpreadAngle; }
    bool IsAmmoFull();
    EWeaponType GetWeaponType() const { return WeaponType; }
    FTransform GetMuzzleTransform() const;
    AV_PlayerCharacter* GetOwningPlayer() const { return OwningPlayer.Get(); }
    UTexture2D* GetAmmoIcon() const { return WeaponIcon; }
    
    UFUNCTION(BlueprintCallable)
    float GetShootCD() const { return ShootCD; }

    UFUNCTION(BlueprintCallable)
    int32 GetBulletsPerShot() const { return BulletsPerShot; }

protected:

    // ====== override func ======

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // ======= 内部辅助方法 =======
    
    void HideSelf(bool bHide);

    // ======= 内部变量 =======
    
    UPROPERTY(Replicated)
    TWeakObjectPtr<AV_PlayerCharacter> OwningPlayer;

    UPROPERTY()
    TObjectPtr<UV_WeaponASC> WeaponASC;
    
    UPROPERTY()
    TObjectPtr<UV_WeaponAttributeSet> WeaponAttributeSet;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USkeletalMeshComponent> WeaponMesh;

    FGameplayAbilitySpecHandle FireAbilityHandle;
    float SpreadAngle{.1f};   // only exist in server
    
    // ======= 配置变量 =======
    
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    EWeaponType WeaponType;

    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;
    
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TSubclassOf<UGameplayEffect> InitEffectClass;

    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    TObjectPtr<UTexture2D> WeaponIcon;

    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    float ShootCD{.1f};

    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    int32 BulletsPerShot{1};

    // 后坐力设置
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    float SpreadAngleResetSpeed{.1f};
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    float SpreadAnglePerShot{.1f};
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    float MaxSpreadAngle{30.f};
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    float MinSpreadAngle{.1f};
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    bool bShowDebugCone{false};
    UPROPERTY(EditDefaultsOnly, Category=Weapon)
    float DebugConeDis{500.f};

};