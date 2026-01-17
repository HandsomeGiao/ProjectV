#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Data/Inv_RealItemData.h"
#include "Inv_InventoryWidgetBase.generated.h"

class UInv_ItemOptionsWidget;
class UInv_InventoryEntry;
class UPanelWidget;
class UUniformGridPanel;

UCLASS(Blueprintable, BlueprintType)
class INVENTORYSYSTEM_API UInv_InventoryWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    // ========== 修改接口 ==========

    void AddItem(const FInv_RealItemData& ItemData);
    void UpdateItem(const FInv_RealItemData& ItemData);
    void RemoveItem(const FGuid& ItemId);
    void InitializeInventory(int32 InTotalSlots, int32 InColumnsPerRow);
    void UpdateInventory(const TArray<FInv_RealItemData>& ItemDataArray);
    void UpdateSlot(const FInv_RealItemData& ItemData);
    void ClearSlot(int32 SlotIndex);
    void ClearAllSlots();

    // ========== 查询接口 ==========
    int32 GetSlotCount() const { return InventoryEntries.Num(); }
    int32 FindIdxByItemID(const FGuid& ItemId) const;
    bool IsValidSlotIndex(int32 SlotIndex) const;
    const FInv_RealItemData* GetItemDataBySlotIndex(int32 SlotIndex) const;

    // ========= 广播操作事件 ==========
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDrop, FGuid, SourceItemId, FGuid, TargetItemId);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemMove, int32, SourceIdx, int32, TargetIdx);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSplit, FGuid, ItemId, int32, SplitCount);

    FOnItemDrop OnItemDrop;
    FOnItemMove OnItemMove;
    FOnItemSplit OnItemSplit;

protected:
    // ========== Entry响应函数 ======
    UFUNCTION()
    void OnItemDroppedFunc(FGuid SourceItemId, FGuid TargetItemId);
    UFUNCTION()
    void OnItemMoveFunc(int32 SourceIdx, int32 TargetIdx);
    UFUNCTION()
    void OnItemOptionsFunc(int32 WidgetIndex);
    UFUNCTION()
    void OnItemSplitFunc(FGuid ItemId, int32 SplitCount);

    // ========== 配置参数 ==========
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory System|UI")
    TSubclassOf<UInv_InventoryEntry> EntryWidgetClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UUniformGridPanel* InventoryGrid;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory System|UI")
    TSubclassOf<UInv_ItemOptionsWidget> ItemOptionsWidgetClass;

private:
    // ========== 内部数据 ==========

    UPROPERTY()
    TArray<UInv_InventoryEntry*> InventoryEntries;

    TWeakObjectPtr<UInv_ItemOptionsWidget> ItemOptionsWidgetInstance;

    int32 ColumnsPerRow;
    int32 TotalSlots;

    // ========== 内部辅助方法 ==========

    UInv_InventoryEntry* CreateEntryWidget();
};