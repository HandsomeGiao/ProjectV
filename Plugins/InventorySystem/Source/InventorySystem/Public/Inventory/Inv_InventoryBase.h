#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Items/FastArray/Inv_ItemFastArray.h"
#include "GameplayTagContainer.h"
#include "Inv_InventoryBase.generated.h"

class UInv_InventoryWidgetBase;

UCLASS(ClassGroup = (InventorySystem), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInv_InventoryBase : public UActorComponent
{
    GENERATED_BODY()

public:
    UInv_InventoryBase();

    // ========= Override Func ========

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ========== 查询接口 ==========

    const FInv_RealItemData* FindItemById(const FGuid& ItemId) const;
    const FInv_RealItemData* FindItemByIndex(int32 Index) const;
    bool ContainsItem(const FGuid& ItemId) const;
    int32 GetItemCount() const;
    bool IsEmpty() const;
    TArray<FGuid> GetAllItemIds() const;
    bool IsFull() const { return MaxSlots > 0 && GetItemCount() >= MaxSlots; }
    int32 GetRemainingSlots() const { return MaxSlots > 0 ? MaxSlots - GetItemCount() : INT32_MAX; }
    int32 GetMaxSlots() const { return MaxSlots; }

    // ========== 客户端调用函数 ===========

    void SelectItem(int32 Idx);
    void UseItem();

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory System")
    void ServerPickupItem(AActor* ItemActor);
    UFUNCTION(Server, Reliable)
    void ServerOnItemDroppedFunc(FGuid SourceItemId, FGuid TargetItemId);
    UFUNCTION(Server, Reliable)
    void ServerOnItemSplitFunc(FGuid ItemId, int32 SplitCount);
    UFUNCTION(Server, Reliable)
    void ServerAddSelectGA(const TArray<TSubclassOf<UGameplayAbility>>& AbilityClasses);
    UFUNCTION(Server, Reliable)
    void ServerRemoveSelectGA();

    // ========== 委托定义 ==========
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const FInv_RealItemData &, ItemData);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemChangedDelegate, const FInv_RealItemData &, ItemData);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, FGuid, ItemId);

    // ========== 修饰性事件 =======
    UPROPERTY(BlueprintAssignable, Category = "Inventory System|Events")
    FOnItemAddedDelegate OnItemAddedEvent;
    UPROPERTY(BlueprintAssignable, Category = "Inventory System|Events")
    FOnItemChangedDelegate OnItemChangedEvent;
    UPROPERTY(BlueprintAssignable, Category = "Inventory System|Events")
    FOnItemRemovedDelegate OnItemRemovedEvent;

protected:
    // ========== 修改接口（仅服务端）==========

    virtual bool TryAddItemInstanceIndividually(const FInv_RealItemData& ItemData);
    int32 TryAddItemInstanceByStacking(const FInv_RealItemData& RealItemData);
    virtual bool TryRemoveItem(const FGuid& ItemId);
    virtual bool UpdateItem(const FGuid& ItemId, const FInv_RealItemData& NewItemData);
    virtual void ClearAllItems();
    virtual bool TryPickupItem(AActor* ItemActor);

    // ========== 配置属性 ==========

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System",
        meta = (ClampMin = "0", UIMin = "0", UIMax = "200"))
    int32 MaxSlots{40};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System")
    int32 PerRowCount{5};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System",
        meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "1000.0"))
    float PickupRange{300.0f};

    // =============== 响应ItemList事件 ==========
    FDelegateHandle OnItemAddedHandle;
    FDelegateHandle OnItemChangedHandle;
    FDelegateHandle OnItemRemovedHandle;

    UFUNCTION()
    void OnItemAdded(const FInv_RealItemData& ItemData);
    UFUNCTION()
    void OnItemChanged(const FInv_RealItemData& ItemData);
    UFUNCTION()
    void OnItemRemoved(FGuid ItemId);

    // ============ 响应UI事件 ===========

    UFUNCTION()
    void OnItemMoved(int32 SourceIdx, int32 TargetIdx);

    // ========== 条件检查接口 ==========

    virtual bool CanAddItemEntirely(const FInv_RealItemData& ItemData) const;
    bool IsItemTypeAllowed(const FInv_RealItemData& ItemData) const;
    virtual bool CanRemoveItem(const FGuid& ItemId) const;

    UPROPERTY(EditAnywhere, Category = "Inventory System")
    bool bUseItemTypeFilter{false};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Inventory System")
    FGameplayTagContainer AllowedItemTypes;

    class UInv_VirtualItemDataSubsystem* GetVirtualItemDataSubsystem() const;

private:
    // ========== FastArray 数据 ==========
    UPROPERTY(Replicated)
    FInv_ItemList ItemList;

    // ========= 关联UI窗口 =========
    UPROPERTY(EditAnywhere, Category = "Inventory System|UI")
    TSubclassOf<UInv_InventoryWidgetBase> InventoryWidgetClass;
    TWeakObjectPtr<UInv_InventoryWidgetBase> InventoryWidgetInstance;

    // ========= 辅助变量 ==========

    // 当前选择的物品Idx，只存在于客户端,小心处理这个变量！因为物品可能会被外部修改Idx,以及可能消失
    int32 CurrentSelectedIndex{INDEX_NONE};

    // 这个变量只存在与服务器端
    TArray<FGameplayAbilitySpecHandle> SelectAbilityHandles;

    // ========= 内部辅助方法 ==========
    void BindFastArrayDelegates();
    void UnbindFastArrayDelegates();
    void CreateInvWidget();
};