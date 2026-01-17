#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Items/Data/Inv_RealItemData.h"
#include "Inv_ItemFastArray.generated.h"

struct FInv_ItemList;

USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FInv_ItemFastArrayItem : public FFastArraySerializerItem
{
    GENERATED_BODY()

    FInv_ItemFastArrayItem()
    {
    }

    explicit FInv_ItemFastArrayItem(const FInv_RealItemData &InItemData) : RealItemData(InItemData)
    {
    }

    // ========== 数据成员 ==========

    UPROPERTY()
    FInv_RealItemData RealItemData;

    // ========== 辅助方法 ==========

    FORCEINLINE FGuid GetItemId() const { return RealItemData.RealItemId; }
};

USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FInv_ItemList : public FFastArraySerializer
{
    GENERATED_BODY()

    // ======= init ========
    
    FInv_ItemList() : OwnerComponent(nullptr)
    {
    }

    explicit FInv_ItemList(UActorComponent *InComp) : OwnerComponent(InComp)
    {
    }

    void SetOwnerComponent(UActorComponent *InComp)
    {
        OwnerComponent = InComp;
    }

    // ========== 数组变更回调 =======
    void PreReplicatedRemove(const TArrayView<int32> &RemovedIndices, int32 FinalSize);
    void PostReplicatedAdd(const TArrayView<int32> &AddedIndices, int32 FinalSize);
    void PostReplicatedChange(const TArrayView<int32> &ChangedIndices, int32 FinalSize);

    // ========== 委托定义 ==========

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAdded, const FInv_RealItemData & /*ItemData*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChanged, const FInv_RealItemData & /*ItemData*/);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FGuid /*ItemId*/);

    FOnItemAdded OnItemAdded;
    FOnItemChanged OnItemChanged;
    FOnItemRemoved OnItemRemoved;

    // ========= 啥玩意 =======

    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParms)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FInv_ItemFastArrayItem, FInv_ItemList>(Items, DeltaParms,
            *this);
    }

    // ========== 物品管理 API(Server Only) ==========

    bool TryAddNewItem(const FInv_RealItemData &ItemData);
    int32 TryStackItem(const FInv_RealItemData &NewRealItem);
    void TryDropItem(FGuid SourceItemId, FGuid TargetItemId);
    bool TrySplitItem(FGuid ItemId, int32 SplitCount);
    bool RemoveItem(const FGuid &ItemId);
    bool RemoveItemAt(int32 Index);
    bool UpdateItem(const FGuid &ItemId, const FInv_RealItemData &NewItemData);
    void ClearAllItems();

    // ========== 查询 API（客户端和服务端都可用）==========

    const FInv_RealItemData *FindItem(const FGuid &ItemId) const;
    TArray<FGuid> GetAllItemIds() const;
    int32 Num() const { return Items.Num(); }
    bool IsEmpty() const { return Items.IsEmpty(); }
    bool ContainRealItem(const FGuid &ItemId) const;

    // ========== 内部辅助方法 ==========

    int32 IndexOfItem(const FGuid &ItemId) const;
    bool IsServer() const;

private:
    // ========== 数据成员 ==========

    UPROPERTY()
    TArray<FInv_ItemFastArrayItem> Items;

    TWeakObjectPtr<UActorComponent> OwnerComponent;
};

// ========== FastArray 序列化支持 ==========

template <>
struct TStructOpsTypeTraits<FInv_ItemList> : public TStructOpsTypeTraitsBase2<FInv_ItemList>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};