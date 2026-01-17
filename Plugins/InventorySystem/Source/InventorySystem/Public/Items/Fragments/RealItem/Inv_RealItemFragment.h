#pragma once

#include "CoreMinimal.h"
#include "Inv_RealItemFragment.generated.h"

// TODO: 暂时还没有使用到RealItemFragment

USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FInv_BaseRealItemFragment
{
    GENERATED_BODY()

    // 有派生类，析构函数必须为virtual
    virtual ~FInv_BaseRealItemFragment() = default;
};