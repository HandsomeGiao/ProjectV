#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Data/Inv_RealItemData.h"
#include "Inv_ItemComponent.generated.h"

/**
 * 物品组件 - 标识一个Actor是可拾取的物品，标识了物品的基本行为和属性
 */
UCLASS(ClassGroup = (InventorySystem), meta = (BlueprintSpawnableComponent), Blueprintable)
class INVENTORYSYSTEM_API UInv_ItemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInv_ItemComponent();

    // ========== override func ==========
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;

    // ========== 查询接口 ==========

    const FInv_RealItemData& GetItemData() const { return RealItemData; }
    FInv_RealItemData& GetItemDataMutable() { return RealItemData; }
    const FInv_VirtualItemData* GetVirtualItemData() const;
    UTexture2D* GetItemIcon() const;

    bool CanBePickedUp() const;
    bool IsPickedUp() const { return bIsPickedUp; }

    // ========== 修改接口 ==========
    
    void SetItemData(const FInv_RealItemData& NewItemData);
    void MarkAsPickedUp();
    void OnPickedUp(AActor* Picker);

protected:
    // ========== 配置变量 ==========

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System|Lifecycle")
    bool bDestroyOnPickup{true};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System|Lifecycle",
        meta = (ClampMin = "0.0", EditCondition = "bDestroyOnPickup"))
    float DestroyDelay{0.1f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System|Lifecycle",
        meta = (ClampMin = "0.0"))
    float LifeSpan{0.0f};

    // ========== 事件回调 ==========

    UFUNCTION(BlueprintNativeEvent, Category = "Inventory System|Item")
    void OnItemDataChanged();

    UFUNCTION(BlueprintNativeEvent, Category = "Inventory System|Item")
    void OnItemDestroyed();

private:
    // ========== 私有数据 ==========

    UPROPERTY(ReplicatedUsing = OnRep_ItemData, EditAnywhere, BlueprintReadWrite, Category = "Inventory System|Item",
        meta = (AllowPrivateAccess = "true"), Replicated)
    FInv_RealItemData RealItemData;

    UPROPERTY(Replicated)
    bool bIsPickedUp{false};

    FTimerHandle DestroyTimerHandle;

    // ========== 内部方法 ==========

    void DestroyAfterDelay();

    UFUNCTION()
    void OnRep_ItemData();
};