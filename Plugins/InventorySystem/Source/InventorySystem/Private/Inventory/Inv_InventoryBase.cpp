#include "Inventory/Inv_InventoryBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Engine/World.h"
#include "InventorySystem.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/Inv_VirtualItemDataSubsystem.h"
#include "Widgets/Inv_InventoryWidgetBase.h"

UInv_InventoryBase::UInv_InventoryBase()
{
    SetIsReplicatedByDefault(true);

    PrimaryComponentTick.bCanEverTick = false;
}

void UInv_InventoryBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInv_InventoryBase, ItemList);
}

void UInv_InventoryBase::CreateInvWidget()
{
    // 创建UI窗口,需要注意,只会在客户端创建UI窗口
    if (GetNetMode() != NM_DedicatedServer && IsValid(InventoryWidgetClass))
    {
        InventoryWidgetInstance = CreateWidget<UInv_InventoryWidgetBase>(GetWorld()->GetFirstPlayerController(),
                                                                         InventoryWidgetClass);
        if (InventoryWidgetInstance.IsValid())
        {
            InventoryWidgetInstance->AddToViewport();
            InventoryWidgetInstance->InitializeInventory(MaxSlots, PerRowCount);
            InventoryWidgetInstance->OnItemDrop.AddDynamic(this, &ThisClass::ServerOnItemDroppedFunc);
            InventoryWidgetInstance->OnItemSplit.AddDynamic(this, &ThisClass::ServerOnItemSplitFunc);
            InventoryWidgetInstance->OnItemMove.AddDynamic(this, &ThisClass::OnItemMoved);
        }
        else
        {
            UE_LOG(LogInventorySystem, Error,
                   TEXT("UInv_InventoryBase::BeginPlay: Failed to create InventoryWidgetInstance"));
        }
    }
}

void UInv_InventoryBase::BeginPlay()
{
    Super::BeginPlay();

    // 绑定 FastArray 委托
    BindFastArrayDelegates();
    ItemList.SetOwnerComponent(this);

    UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::BeginPlay: Inventory initialized on %s"),
           GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"));

    CreateInvWidget();
}

void UInv_InventoryBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 解绑委托
    UnbindFastArrayDelegates();

    Super::EndPlay(EndPlayReason);
}

const FInv_RealItemData* UInv_InventoryBase::FindItemById(const FGuid& ItemId) const
{
    return ItemList.FindItem(ItemId);
}

const FInv_RealItemData* UInv_InventoryBase::FindItemByIndex(int32 Index) const
{
    return IsValid(InventoryWidgetInstance.Get()) ? InventoryWidgetInstance->GetItemDataBySlotIndex(Index) : nullptr;
}

bool UInv_InventoryBase::ContainsItem(const FGuid& ItemId) const
{
    return ItemList.ContainRealItem(ItemId);
}

int32 UInv_InventoryBase::GetItemCount() const
{
    return ItemList.Num();
}

bool UInv_InventoryBase::IsEmpty() const
{
    return ItemList.IsEmpty();
}

TArray<FGuid> UInv_InventoryBase::GetAllItemIds() const
{
    return ItemList.GetAllItemIds();
}

bool UInv_InventoryBase::TryAddItemInstanceIndividually(const FInv_RealItemData& ItemData)
{
    // 仅在服务端执行
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::TryAddItemInstance: Can only add items on server! Owner: %s"),
               *GetOwner()->GetName());
        return false;
    }

    // 验证是否可以完全添加
    if (!CanAddItemEntirely(ItemData))
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::TryAddItemInstance: CanAddItem check failed"));
        return false;
    }

    // 添加到 FastArray
    const bool bSuccess = ItemList.TryAddNewItem(ItemData);

    if (bSuccess)
    {
        UE_LOG(LogInventorySystem, Display,
               TEXT("UInv_InventoryBase::TryAddItemInstance: Successfully added item '%s'"),
               *ItemData.RealItemId.ToString());
    }

    return bSuccess;
}

int32 UInv_InventoryBase::TryAddItemInstanceByStacking(const FInv_RealItemData& RealItemData)
{
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::TryAddItemInstancePartially: Can only add items on server! Owner: %s"),
               *GetOwner()->GetName());
        return 0;
    }

    return ItemList.TryStackItem(RealItemData);
}

bool UInv_InventoryBase::TryRemoveItem(const FGuid& ItemId)
{
    // 仅在服务端执行
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::TryRemoveItem: Can only remove items on server! Owner: %s"),
               *GetOwner()->GetName());
        return false;
    }

    // 验证是否可以移除
    if (!CanRemoveItem(ItemId))
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::TryRemoveItem: CanRemoveItem check failed"));
        return false;
    }

    // 从 FastArray 移除
    const bool bSuccess = ItemList.RemoveItem(ItemId);

    if (bSuccess)
    {
        UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::TryRemoveItem: Successfully removed item '%s'"),
               *ItemId.ToString());
    }
    else
    {
        UE_LOG(LogInventorySystem, Error, TEXT("UInv_InventoryBase::TryRemoveItem: Failed to remove item '%s'"),
               *ItemId.ToString());
    }

    return bSuccess;
}

void UInv_InventoryBase::ClearAllItems()
{
    // 仅在服务端执行
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::ClearAllItems: Can only clear items on server! Owner: %s"),
               *GetOwner()->GetName());
        return;
    }

    ItemList.ClearAllItems();

    UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::ClearAllItems: Cleared all items"));
}

bool UInv_InventoryBase::UpdateItem(const FGuid& ItemId, const FInv_RealItemData& NewItemData)
{
    // 仅在服务端执行
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Error, TEXT("UInv_InventoryBase::UpdateItem: Can only execute on server! Owner: %s"),
               *GetOwner()->GetName());
        return false;
    }

    // 验证 ItemId 是否有效
    if (!ItemId.IsValid())
    {
        UE_LOG(LogInventorySystem, Error, TEXT("UInv_InventoryBase::UpdateItem: Invalid ItemId (GUID is zero)"));
        return false;
    }

    // 验证 NewItemData.RealItemId 是否有效
    if (!NewItemData.RealItemId.IsValid())
    {
        UE_LOG(LogInventorySystem, Error, TEXT("UInv_InventoryBase::UpdateItem: Invalid RealItemId in NewItemData"));
        return false;
    }

    // 验证 ID 一致性
    if (NewItemData.RealItemId != ItemId)
    {
        UE_LOG(LogInventorySystem, Error,
               TEXT("UInv_InventoryBase::UpdateItem: Item ID mismatch! Requested: %s, Data: %s"), *ItemId.ToString(),
               *NewItemData.RealItemId.ToString());
        return false;
    }

    // 执行更新
    const bool bSuccess = ItemList.UpdateItem(ItemId, NewItemData);

    if (bSuccess)
    {
        UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::UpdateItem: Successfully updated item '%s'"),
               *ItemId.ToString());
    }
    else
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::UpdateItem: Failed to update item '%s'"),
               *ItemId.ToString());
    }

    return bSuccess;
}

bool UInv_InventoryBase::TryPickupItem(AActor* ItemActor)
{
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::TryPickupItem: Can only execute on server"));
        return false;
    }

    if (!IsValid(ItemActor))
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::TryPickupItem: Invalid ItemActor"));
        return false;
    }

    UInv_ItemComponent* ItemComp = ItemActor->FindComponentByClass<UInv_ItemComponent>();
    if (!ItemComp)
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::TryPickupItem: ItemActor has no ItemComponent"));
        return false;
    }

    if (!ItemComp->CanBePickedUp())
    {
        UE_LOG(LogInventorySystem, Display,
               TEXT("UInv_InventoryBase::TryPickupItem: ItemComponent.CanBePickedUp() returned false"));
        return false;
    }

    FInv_RealItemData& ItemData = ItemComp->GetItemDataMutable();

    // pick up at least one of them ?
    bool bSuccess = false;

    // 尝试部分添加物品
    // 对于ItemComp的所有数据更改均放在本函数中,所以以下的TryXXX函数不对传入的ItemData进行更改.
    const int32 TotalStackCount = TryAddItemInstanceByStacking(ItemData);
    if (TotalStackCount > 0)
    {
        bSuccess = true;
        ItemData.StackCount -= TotalStackCount;
        UE_LOG(LogInventorySystem, Display,
               TEXT("UInv_InventoryBase::TryPickupItem Partially: Picked up %d items from '%s', %d remaining"),
               TotalStackCount,
               *ItemActor->GetName(), ItemData.StackCount);
    }

    // 尝试不适用堆叠添加物品
    if (ItemData.StackCount > 0)
    {
        // TODO:这里有BUG，如果地上的物品的堆叠数量超过了最大堆叠数量，也会直接将其作为一个整体捡起，导致物品栏中的
        // 堆叠数量超过最大堆叠数量，但是在设置地上物品堆叠数量时进行控制即可。
        if (TryAddItemInstanceIndividually(ItemData))
        {
            UE_LOG(LogInventorySystem, Display,
                   TEXT("UInv_InventoryBase::TryPickupItem Entirelly: Successfully picked up %d items from '%s'"),
                   ItemData.StackCount,
                   *ItemActor->GetName());

            ItemData.StackCount = 0;
            bSuccess = true;
        }
        else
        {
            UE_LOG(LogInventorySystem, Log,
                   TEXT("UInv_InventoryBase::TryPickupItem: Could not pick up '%s' Individually, %d remaining"),
                   *ItemActor->GetName(), ItemData.StackCount);
        }
    }

    if (bSuccess)
        ItemComp->OnPickedUp(GetOwner());

    return bSuccess;
}

void UInv_InventoryBase::SelectItem(int32 Idx)
{
    if (Idx == CurrentSelectedIndex)
    {
        UE_LOG(LogInventorySystem, Log,
               TEXT("UInv_InventoryBase::SelectItem: Item at index %d is already selected"), Idx);
        return;
    }

    if (!IsValid(InventoryWidgetInstance.Get()))
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::SelectItem: InventoryWidgetInstance is invalid"));
        return;
    }

    if (!InventoryWidgetInstance->IsValidSlotIndex(Idx))
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::SelectItem: Invalid slot index %d"), Idx);
        return;
    }

    const FInv_RealItemData* ItemData = FindItemByIndex(Idx);
    if (!ItemData)
    {
        UE_LOG(LogInventorySystem, Log,
               TEXT("UInv_InventoryBase::SelectItem: No item found at index %d"), Idx);
        return;
    }

    if (CurrentSelectedIndex != INDEX_NONE)
    {
        ServerRemoveSelectGA();
    }

    CurrentSelectedIndex = Idx;
    // 如果有选择效果Fragment，则调用其 SelectedByActor 方法
    const FInv_SelectEffectFragment* SelectEffectFragment =
            ItemData->GetVirtualItemData()->GetFragmentOfType<FInv_SelectEffectFragment>();
    if (SelectEffectFragment)
        SelectEffectFragment->SelectedByActor(GetOwner());

    UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::SelectItem: Selected item at index %d"), Idx);
}

void UInv_InventoryBase::UseItem()
{
    if (CurrentSelectedIndex == INDEX_NONE)
    {
        UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::UseItem: No item selected"));
        return;
    }

    const FInv_RealItemData* ItemData = FindItemByIndex(CurrentSelectedIndex);
    // TODO: use item
    if (!ItemData)
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::UseItem: No item data found at selected index %d"), CurrentSelectedIndex);
        return;
    }
    const FInv_UsedFragment* UsedFragment =
            ItemData->GetVirtualItemData()->GetFragmentOfType<FInv_UsedFragment>();
    if (!UsedFragment)
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::UseItem: Item '%s' has no UsedFragment"), *ItemData->RealItemId.ToString());
        return;
    }
    UsedFragment->UsedByActor(GetOwner());
}

void UInv_InventoryBase::ServerRemoveSelectGA_Implementation()
{
    if (!SelectAbilityHandles.IsEmpty())
    {
        AActor* OwnerActor = GetOwner();
        if (!IsValid(OwnerActor))
        {
            UE_LOG(LogInventorySystem, Warning,
                   TEXT("UInv_InventoryBase::ServerRemoveGA_Implementation: Owner is invalid"));
            return;
        }

        IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor);
        if (!ASI)
        {
            UE_LOG(LogInventorySystem, Warning,
                   TEXT(
                       "UInv_InventoryBase::ServerRemoveGA_Implementation: Owner does not implement AbilitySystemInterface"
                   ));
            return;
        }

        UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
        if (!IsValid(ASC))
        {
            UE_LOG(LogInventorySystem, Warning,
                   TEXT("UInv_InventoryBase::ServerRemoveGA_Implementation: AbilitySystemComponent is invalid"));
            return;
        }

        for (FGameplayAbilitySpecHandle& Handle : SelectAbilityHandles)
        {
            if (Handle.IsValid())
                ASC->ClearAbility(Handle);
        }
        SelectAbilityHandles.Empty();
    }
}

void UInv_InventoryBase::ServerAddSelectGA_Implementation(const TArray<TSubclassOf<UGameplayAbility>>& AbilityClasses)
{
    AActor* OwnerActor = GetOwner();
    if (!IsValid(OwnerActor))
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_InventoryBase::ServerAddGA_Implementation: Owner is invalid"));
        return;
    }

    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor);
    if (!ASI)
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::ServerAddGA_Implementation: Owner does not implement AbilitySystemInterface"));
        return;
    }

    UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
    if (!IsValid(ASC))
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::ServerAddGA_Implementation: AbilitySystemComponent is invalid"));
        return;
    }

    // remove old select abilities
    for (FGameplayAbilitySpecHandle& Handle : SelectAbilityHandles)
    {
        if (Handle.IsValid())
            ASC->ClearAbility(Handle);
    }
    SelectAbilityHandles.Empty();
    for (TSubclassOf<UGameplayAbility> AbilityClass : AbilityClasses)
    {
        if (IsValid(AbilityClass))
        {
            FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
            FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
            SelectAbilityHandles.Add(Handle);
        }
    }
}

void UInv_InventoryBase::ServerPickupItem_Implementation(AActor* ItemActor)
{
    TryPickupItem(ItemActor);
}

bool UInv_InventoryBase::ServerPickupItem_Validate(AActor* ItemActor)
{
    // 基本验证：Actor 必须有效
    if (!ItemActor)
    {
        UE_LOG(LogInventorySystem, Error, TEXT("UInv_InventoryBase::ServerPickupItem_Validate: Invalid ItemActor"));
        return false;
    }

    // 必须包含 ItemComponent
    UInv_ItemComponent* ItemComp = ItemActor->FindComponentByClass<UInv_ItemComponent>();
    if (!ItemComp)
    {
        UE_LOG(LogInventorySystem, Error,
               TEXT("UInv_InventoryBase::ServerPickupItem_Validate: ItemActor has no ItemComponent"));
        return false;
    }

    return true;
}

void UInv_InventoryBase::OnItemAdded(const FInv_RealItemData& ItemData)
{
    if (IsFull())
        UE_LOG(LogInventorySystem, Display,
           TEXT("UInv_InventoryBase::OnItemAdded: Inventory is full after adding item '%s'"),
           *ItemData.RealItemId.ToString());
    OnItemAddedEvent.Broadcast(ItemData);

    if (InventoryWidgetInstance.IsValid())
        InventoryWidgetInstance->AddItem(ItemData);

    UE_LOG(LogInventorySystem, Log, TEXT("UInv_InventoryBase::OnItemAdded: Item '%s' added (Role: %s)"),
           *ItemData.RealItemId.ToString(),
           GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"));
}

void UInv_InventoryBase::OnItemChanged(const FInv_RealItemData& ItemData)
{
    OnItemChangedEvent.Broadcast(ItemData);

    if (InventoryWidgetInstance.IsValid())
        InventoryWidgetInstance->UpdateItem(ItemData);

    UE_LOG(LogInventorySystem, Display, TEXT("UInv_InventoryBase::OnItemChanged: Item '%s' changed (Role: %s)"),
           *ItemData.RealItemId.ToString(),
           GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"));
}

void UInv_InventoryBase::OnItemRemoved(FGuid ItemId)
{
    OnItemRemovedEvent.Broadcast(ItemId);

    if (InventoryWidgetInstance.IsValid())
    {
        if (CurrentSelectedIndex != INDEX_NONE &&
            CurrentSelectedIndex == InventoryWidgetInstance->FindIdxByItemID(ItemId))
        {
            ServerRemoveSelectGA();
            UE_LOG(LogInventorySystem, Log,
                   TEXT("UInv_InventoryBase::OnItemRemoved: Removed selected item '%s', clearing selection"),
                   *ItemId.ToString());
            CurrentSelectedIndex = INDEX_NONE;
        }

        InventoryWidgetInstance->RemoveItem(ItemId);
    }

    UE_LOG(LogInventorySystem, Display, TEXT("UInv_InventoryBase::OnItemRemoved: Item '%s' removed (Role: %s)"),
           *ItemId.ToString(),
           GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"));
}

void UInv_InventoryBase::OnItemMoved(int32 SourceIdx, int32 TargetIdx)
{
    // 在Widget中，如果将一个物品移动到空的槽位，则由于性能考虑没有调用在服务器上移除再添加的逻辑，而是直接更改了槽位对应的物品数据
    // 所以在这里添加了一个额外的OnItemMoved回调。
    if (SourceIdx == CurrentSelectedIndex)
    {
        ServerRemoveSelectGA();
    }
}

bool UInv_InventoryBase::CanAddItemEntirely(const FInv_RealItemData& ItemData) const
{
    if (IsFull())
    {
        UE_LOG(LogInventorySystem, Warning, TEXT("UInv_SlottedInventoryComponent::CanAddItem: Inventory is full"));
        return false;
    }

    if (bUseItemTypeFilter && !IsItemTypeAllowed(ItemData))
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::CanAddItemEntirely: Item type '%s' not allowed in this inventory"),
               *ItemData.GetVirtualItemData()->ItemTag.ToString());
        return false;
    }

    return true;
}

bool UInv_InventoryBase::CanRemoveItem(const FGuid& ItemId) const
{
    return ItemList.ContainRealItem(ItemId);
}

void UInv_InventoryBase::BindFastArrayDelegates()
{
    // 绑定 FastArray 委托到本地方法
    OnItemAddedHandle = ItemList.OnItemAdded.AddUObject(this, &UInv_InventoryBase::OnItemAdded);
    OnItemChangedHandle = ItemList.OnItemChanged.AddUObject(this, &UInv_InventoryBase::OnItemChanged);
    OnItemRemovedHandle = ItemList.OnItemRemoved.AddUObject(this, &UInv_InventoryBase::OnItemRemoved);

    UE_LOG(LogInventorySystem, Display, TEXT("UInv_InventoryBase::BindFastArrayDelegates: Delegates bound"));
}

void UInv_InventoryBase::UnbindFastArrayDelegates()
{
    // 解绑委托
    if (OnItemAddedHandle.IsValid())
    {
        ItemList.OnItemAdded.Remove(OnItemAddedHandle);
        OnItemAddedHandle.Reset();
    }

    if (OnItemChangedHandle.IsValid())
    {
        ItemList.OnItemChanged.Remove(OnItemChangedHandle);
        OnItemChangedHandle.Reset();
    }

    if (OnItemRemovedHandle.IsValid())
    {
        ItemList.OnItemRemoved.Remove(OnItemRemovedHandle);
        OnItemRemovedHandle.Reset();
    }

    UE_LOG(LogInventorySystem, Display, TEXT("UInv_InventoryBase::UnbindFastArrayDelegates: Delegates unbound"));
}

UInv_VirtualItemDataSubsystem* UInv_InventoryBase::GetVirtualItemDataSubsystem() const
{
    if (!GetWorld())
    {
        return nullptr;
    }

    UGameInstance* GameInstance = GetWorld()->GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }

    return GameInstance->GetSubsystem<UInv_VirtualItemDataSubsystem>();
}

bool UInv_InventoryBase::IsItemTypeAllowed(const FInv_RealItemData& ItemData) const
{
    if (!bUseItemTypeFilter)
        return true;

    const FInv_VirtualItemData* VirtualData = ItemData.GetVirtualItemData();
    if (!VirtualData)
    {
        return false;
    }

    return VirtualData->ItemTag.MatchesAny(AllowedItemTypes);
}

void UInv_InventoryBase::ServerOnItemSplitFunc_Implementation(FGuid ItemId, int32 SplitCount)
{
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("UInv_InventoryBase::OnItemSplitFunc: Can only execute on server! Owner: %s"),
               *GetOwner()->GetName());
        return;
    }

    //背包已经满了,无法再分割物品
    if (IsFull())
    {
        UE_LOG(LogInventorySystem, Display,
               TEXT("UInv_InventoryBase::OnItemSplitFunc: Inventory is full, cannot split item '%s'"),
               *ItemId.ToString());
        return;
    }

    if (!ItemList.TrySplitItem(ItemId, SplitCount))
        UE_LOG(LogInventorySystem, Warning,
           TEXT("UInv_InventoryBase::OnItemSplitFunc: Failed to split item '%s' with count %d"),
           *ItemId.ToString(), SplitCount);
}

void UInv_InventoryBase::ServerOnItemDroppedFunc_Implementation(FGuid SourceItemId, FGuid TargetItemId)
{
    // TODO:这里是只要发生了物品拖拽,就会发送RPC,也许可以通过条件检查进行优化,减少不必要的RPC发送
    ItemList.TryDropItem(SourceItemId, TargetItemId);
}