// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "V_GameMode.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECTV_API AV_GameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AV_GameMode();

    // ======= 修改接口 =======
    UFUNCTION(BlueprintCallable)
    void BeginSeamlessTravel(FString URL);

protected:
    // ======= override func =======
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;
};