// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "V_PC.generated.h"

class UInv_InventoryBase;
struct FPredictProjectilePathParams;
struct FGameplayEventData;
class AV_PlayerCharacter;
class AV_PlayerHUD;
class UV_PlayerMainLayout;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UNiagaraComponent;

UCLASS(BlueprintType)
class PROJECTV_API UV_PlayerConfirmThrowData : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    FVector2D AimDirection;
};

UCLASS()
class PROJECTV_API AV_PC : public APlayerController
{
    GENERATED_BODY()

public:
    AV_PC();

protected:
    // ======== override func ========
    
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnRep_Pawn() override;

private:
    // ========= Input Func ========
    
    void Move(const FInputActionValue& Value);
    void JumpStarted();
    void JumpCompleted();
    void CrouchStarted();
    void CrouchCompleted();
    void PrimaryAttackBegin(const FInputActionValue& Value);
    void PrimaryAttackEnd();
    void Reload();
    void EquipWeapon0();
    void EquipWeapon1();
    void ThrowupCurrentWeapon();
    void ThrowThrowable();
    void EndThrowThrowable();
    void SelectInventorySlot(const FInputActionValue& Value);
    void UseItem();
    void PickupItem();

    // ======== Input Actions & Mapping Context ========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* MoveAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* JumpAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* CrouchAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* PrimaryAttackAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ReloadAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* EquipWeapon0Action = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* EquipWeapon1Action = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ThrowupCurrentWeaponAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ThrowThrowableAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* SelectInventoryItemAction = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* PickupItemAction = nullptr;

    // ======== 内部变量 =============

    TWeakObjectPtr<AV_PlayerCharacter> PlayerChar;
    TWeakObjectPtr<UInv_InventoryBase> InventoryBase;
    TWeakObjectPtr<AActor> DetectedItemActor;
    TWeakObjectPtr<AV_PlayerHUD> PlayerHUD;
    bool bIsThrowing{false};    //正在投掷物品吗

    // ======= 内部辅助方法 ==========
    UFUNCTION(Server, Reliable)
    void Server_PrimaryAttackEnd();

    UFUNCTION(Server, Reliable)
    void Server_ConfirmThrow(FVector2D AimDir);

    void SetInventoryBase();
};