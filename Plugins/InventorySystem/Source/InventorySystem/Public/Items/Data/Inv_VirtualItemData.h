#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Items/Fragments/VirtualItem/Inv_VirtualItemFragment.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_VirtualItemData.generated.h"

/**
 * 代表游戏世界内的某一种物品（虚拟/抽象的物品定义）
 * 例如：代表"香蕉"这种水果，"铁剑"这种武器
 * 存储在 DataTable 中，所有同类物品实例共享这份数据
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FInv_VirtualItemData : public FTableRowBase
{
    GENERATED_BODY()

    // 每一个Tag与一种物品一一对应
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Definition")
    FGameplayTag ItemTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Definition")
    TArray<TInstancedStruct<FInv_VirtualItemFragment>> ItemFragments;

    // 找到第一个符合类型的Fragment
    template <typename T> requires std::derived_from<T, FInv_VirtualItemFragment>
    const T *GetFragmentOfType() const;
    
    // 找到第一个符合类型的Fragment(可变)
    template <typename T> requires std::derived_from<T, FInv_VirtualItemFragment>
    T *GetFragmentOfTypeMutable();
};

template <typename T> requires std::derived_from<T, FInv_VirtualItemFragment>
const T *FInv_VirtualItemData::GetFragmentOfType() const
{
    for (const TInstancedStruct<FInv_VirtualItemFragment> &Fragment : ItemFragments)
    {
        if (const T *FragmentPtr = Fragment.GetPtr<T>()) { return FragmentPtr; }
    }
    return nullptr;
}

template <typename T> requires std::derived_from<T, FInv_VirtualItemFragment>
T *FInv_VirtualItemData::GetFragmentOfTypeMutable()
{
    for (TInstancedStruct<FInv_VirtualItemFragment> &Fragment : ItemFragments)
    {
        if (T *FragmentPtr = Fragment.GetMutablePtr<T>()) { return FragmentPtr; }
    }
    return nullptr;
}