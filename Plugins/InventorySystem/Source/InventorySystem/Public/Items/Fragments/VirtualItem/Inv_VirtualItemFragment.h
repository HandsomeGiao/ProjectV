#pragma once

#include "CoreMinimal.h"
#include "InventorySystem.h"

#include "Inv_VirtualItemFragment.generated.h"

class UGameplayAbility;

// Tips: 这里使用了USTRUCT的派生，需要小心使用，避免值传递带来的截断

USTRUCT(BlueprintType)
struct FInv_VirtualItemFragment
{
    GENERATED_BODY()

    //只要基类析构函数为virtual，则派生类析构函数自动变为virtual
    virtual ~FInv_VirtualItemFragment() = default;
};

// Icon
USTRUCT()
struct FInv_ImageFragment : public FInv_VirtualItemFragment
{
    GENERATED_BODY()

    UTexture2D* GetIcon() const { return Icon; }

private:
    UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
    TObjectPtr<UTexture2D> Icon{nullptr};

    UPROPERTY(EditDefaultsOnly, Category = "InventorySystem")
    FVector2D IconDimensions{44.f, 44.f};
};

USTRUCT()
struct FInv_StackFragment : public FInv_VirtualItemFragment
{
    GENERATED_BODY()

    int32 GetMaxStackCount() const { return MaxStackCount; }

protected:
    UPROPERTY(EditDefaultsOnly, Category="InventorySystem", meta = (ClampMin = "1"))
    int32 MaxStackCount{1};
};

USTRUCT()
struct FInv_UsedFragment : public FInv_VirtualItemFragment
{
    GENERATED_BODY()

    void UsedByActor(AActor* User) const
    {
        UE_LOG(LogInventorySystem, Log, TEXT("FInv_UsedFragment::UsedByActor: Item used by actor '%s'"),
               *User->GetName());
    }
};

/*
 * 将所有的选择effect都塞到这个Struct中，支持同时实现多个效果，且不需要在STRUCT使用virtual函数（这很危险）
 */
USTRUCT()
struct FInv_SelectEffectFragment : public FInv_VirtualItemFragment
{
    GENERATED_BODY()

    void SelectedByActor(AActor* User) const;
    void UnSelectedByActor(AActor* User) const;

private:
    UPROPERTY(EditDefaultsOnly, Category="InventorySystem")
    TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGive{nullptr}; 
};