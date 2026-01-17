// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "V_PlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class PROJECTV_API AV_PlayerState final : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AV_PlayerState();

    // ======= 查找接口 =======
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }
    UFUNCTION(BlueprintCallable)
    FString GetVPlayerName() const { return VPlayerName; }

    // ======= 修改接口 =======
    UFUNCTION(BlueprintCallable)
    void SetVPlayerName(const FString& NewName);

    // ======= 公开委托事件 =======
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVPlayerNameChanged, const FString&, NewName);

    UPROPERTY(BlueprintAssignable)
    FOnVPlayerNameChanged OnVPlayerNameChanged;

private:
    // =======override func =======
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    // ======= 内部变量 =======
    UPROPERTY(VisibleAnywhere, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> ASC;
    UPROPERTY(VisibleAnywhere, Category = "GAS")
    TObjectPtr<UAttributeSet> AttributeSet;

    UPROPERTY(ReplicatedUsing=OnRep_VPlayerName)
    FString VPlayerName; //用于蓝图显示玩家名称

    // ======== 内部函数 ======
    UFUNCTION()
    void OnRep_VPlayerName();
};