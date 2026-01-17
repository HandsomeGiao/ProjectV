#include "Items/Fragments/VirtualItem/Inv_VirtualItemFragment.h"

#include "Inventory/Inv_InventoryBase.h"

void FInv_SelectEffectFragment::SelectedByActor(AActor* User) const
{
    if (!IsValid(User))
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("FInv_SEF_GiveGA::SelectedByActor: User is null"));
        return;
    }

    UE_LOG(LogInventorySystem, Log,
           TEXT("FInv_SEF_GiveGA::SelectedByActor: Actor '%s' selected an item"),
           *User->GetName());

    // Give GA 
    {
        UInv_InventoryBase* InventoryComp = User->FindComponentByClass<UInv_InventoryBase>();
        if (!IsValid(InventoryComp))
        {
            UE_LOG(LogInventorySystem, Warning,
                   TEXT("FInv_SEF_GiveGA::SelectedByActor: User '%s' has no InventoryComponent"),
                   *User->GetName());
            return;
        }

        InventoryComp->ServerAddSelectGA(AbilitiesToGive);
    }
}

void FInv_SelectEffectFragment::UnSelectedByActor(AActor* User) const
{
    if (!IsValid(User))
    {
        UE_LOG(LogInventorySystem, Warning,
               TEXT("FInv_SEF_GiveGA::UnSelectedByActor: User is null"));
        return;
    }

    UE_LOG(LogInventorySystem, Log,
           TEXT("FInv_SEF_GiveGA::UnSelectedByActor: Actor '%s' selected an item"),
           *User->GetName());

    {
        UInv_InventoryBase* InventoryComp = User->FindComponentByClass<UInv_InventoryBase>();
        if (!IsValid(InventoryComp))
        {
            UE_LOG(LogInventorySystem, Warning,
                   TEXT("FInv_SEF_GiveGA::SelectedByActor: User '%s' has no InventoryComponent"),
                   *User->GetName());
            return;
        }
        InventoryComp->ServerRemoveSelectGA();
    }
}