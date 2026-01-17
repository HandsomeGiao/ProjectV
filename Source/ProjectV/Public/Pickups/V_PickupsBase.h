// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chaos/AABB.h"
#include "GameFramework/Actor.h"
#include "V_PickupsBase.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTV_API AV_PickupsBase : public AActor
{
    GENERATED_BODY()

public:
    AV_PickupsBase();

    // ======= 修改接口 =======
    virtual void PickedUpBy(AActor* Actor);
    virtual void Thrownup();

    // ======= override func =======
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    // ======= 内部变量 ========
    FTimerHandle SetPysTimer;
    
    UPROPERTY(EditDefaultsOnly, Category = "Collision")
    UBoxComponent* CollisionComponent;
};