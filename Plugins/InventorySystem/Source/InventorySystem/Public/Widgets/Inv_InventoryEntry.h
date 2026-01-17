#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Data/Inv_RealItemData.h"
#include "Inv_InventoryEntry.generated.h"

class UInv_ItemOptionsWidget;
class UImage;
class UTextBlock;

/**
 * 库存物品格子 Widget
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYSYSTEM_API UInv_InventoryEntry : public UUserWidget
{
    GENERATED_BODY()

public:
    // ========== 修改接口 ==========

    void SetInfo(const FInv_RealItemData& RealItemData);
    void ClearEntry();
    void SetWidgetIndex(int32 InIndex) { WidgetIndex = InIndex; }

    // ========= 查询接口 ==========
    const FInv_RealItemData& GetCurrentItemData() const { return CurrentItemData; }
    FGuid GetCurrentItemId() const { return CurrentItemData.RealItemId; }
    bool IsEmpty() const { return !CurrentItemData.RealItemId.IsValid(); }


    // ========== 多播事件 ==========
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDrop, FGuid, SourceItemId, FGuid, TargetItemId);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemMove, int32, SourceIdx, int32, TargetIdx);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemOptions, int32, WidgetIndex);
    
    FOnItemDrop OnItemDrop;
    FOnItemMove OnItemMove;
    FOnItemOptions OnItemOptions;

protected:
    // ========== UI 组件 ==========

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* ItemIcon;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ItemCount;

    // ========== 拖拽功能 ==========
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                      UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                              UDragDropOperation* InOperation) override;
    
private:
    // ========== 内部数据 ==========

    FInv_RealItemData CurrentItemData;
    int32 WidgetIndex;

    // ========== 内部辅助方法 ==========

    void UpdateIcon(UTexture2D* Icon);
    void UpdateCount();
};