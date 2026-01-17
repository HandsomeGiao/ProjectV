#pragma once

#include "Inv_VirtualItemData.h"
#include "Inv_RealItemData.generated.h"

/**
 * 代表游戏世界中具体存在的物品实例
 * 例如：5个香蕉、一把耐久度80%的铁剑、一个+3附魔的盾牌
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FInv_RealItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Instance")
    FDataTableRowHandle VirtualItemDataHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Instance")
    FGuid RealItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Instance", meta = (ClampMin = "1"))
    int32 StackCount{1};

    // ========== 便捷访问方法 ==========

    const FInv_VirtualItemData* GetVirtualItemData() const
    {
        return VirtualItemDataHandle.GetRow<FInv_VirtualItemData>(TEXT("GetItemData"));
    }
};