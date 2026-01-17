#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Base/V_BaseCharacter.h"
#include "V_PlayerCharacter.generated.h"

class USplineComponent;
struct FPredictProjectilePathParams;
class UV_WeaponASC;
class UV_PlayerASC;
class AV_PlayerHUD;
class AV_WeaponBase;
class UPrimitiveComponent;
struct FHitResult;

UCLASS()
class PROJECTV_API AV_PlayerCharacter : public AV_BaseCharacter
{
    GENERATED_BODY()

public:
    AV_PlayerCharacter();

    // ====== 查询接口 =======

    FTransform GetFollowLocation() const { return WeaponTransform; }
    AV_WeaponBase* GetCurrentWeapon();
    FOnGameplayAttributeValueChange& GetAmmoChangeDelegate();
    float GetReserveAmmo();
    bool IsCurrentWeaponAmmoFull();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // ====== 修改接口 =======

    void EquipWeapon(int8 WeaponIndex);
    void ThrowupCurrentWeapon();

    UFUNCTION(Server, Reliable)
    void Server_GiveGA(TSubclassOf<UGameplayAbility> AbilityClass);

protected:
    // ======= override func =======

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    virtual void OnRep_Controller() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // ======== 内部辅助方法 ==========

    UFUNCTION()
    void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnRep_CurrentWeaponIndex();

    UFUNCTION()
    void OnRep_Weapon0();

    UFUNCTION()
    void OnRep_Weapon1();

    UFUNCTION(Server, Unreliable)
    void Server_UpdateWeaponTransform(const FTransform& NewTransform);

    UFUNCTION(Server, Unreliable)
    void Server_EquipWeapon(int8 WeaponIndex);

    UFUNCTION(Server, Unreliable)
    void Server_ThrowupCurrentWeapon();

    void UpdateAmmoUI();
    void BindWeaponUI();
    void CalculateWeaponLocation(float DeltaTime);
    void InitMoveSpeedFromASC();

    // ======= 内部变量 ========

    TWeakObjectPtr<AV_PlayerHUD> PlayerHUD;
    TWeakObjectPtr<UV_PlayerASC> PlayerASC;
    // 这个数据只保存在Server
    FActiveGameplayEffectHandle ActiveEquipEffectHandle;
    FVector LastCharacterLocation;
    FDelegateHandle MoveSpeedDlgtHandle;

    UPROPERTY(Replicated)
    FTransform WeaponTransform;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_CurrentWeaponIndex)
    int8 CurrentWeaponIndex{-1};

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Weapon0)
    AV_WeaponBase* Weapon0;

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_Weapon1)
    AV_WeaponBase* Weapon1;

    // ======= 配置变量 =======

    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TSubclassOf<UGameplayEffect> EquipWeaponEffectClass;

    UPROPERTY(EditAnywhere, Category="Weapon")
    float WeaponDis{100.f};

    UPROPERTY(EditAnywhere, Category="Weapon")
    float InterpSpeed{15.0f};
};