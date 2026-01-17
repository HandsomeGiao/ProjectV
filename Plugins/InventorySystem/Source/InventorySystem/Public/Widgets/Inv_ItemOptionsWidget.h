#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemOptionsWidget.generated.h"

class UButton;
class USlider;
/**
 * 右键物品时,显示的选项窗口
 */
UCLASS()
class INVENTORYSYSTEM_API UInv_ItemOptionsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ======== 事件 ==========

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSplitInfoChanged, bool, bCanShow);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSplit, FGuid, ItemId, int32, SplitCount);

    FOnSplitInfoChanged OnSplitInfoChanged;
    FOnItemSplit OnItemSplit;

    UFUNCTION()
    void OnSplitBtnClicked();

    // ====== Override Func ======

    virtual void NativeConstruct() override;

    // ====== Modify Func ======

    void SetItemInfo(const FGuid& InItemId, int32 InItemStackCount);

protected:
    
    // ====== UI bind ======
    UPROPERTY(meta = (BindWidget))
    USlider* SplitSlider;
    UPROPERTY(meta = (BindWidget))
    UButton* SplitBtn;

private:
    // ===== private data ======
    FGuid ItemId;
    int32 ItemStackCount;
};