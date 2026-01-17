// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "V_ThrowableInstance.generated.h"

class UGameplayEffect;
class UBoxComponent;

UCLASS(BlueprintType, Blueprintable)
class PROJECTV_API AV_ThrowableInstance : public AActor
{
    GENERATED_BODY()

public:
    AV_ThrowableInstance();

    // ======== 修改接口 ========
    void SetInitialVelocity(const FVector& InitialSpeed);

protected:
    // ======= override func =======
    virtual void BeginPlay() override;

    // ======= 内部变量 =======
    UPROPERTY(EditDefaultsOnly)
    UBoxComponent* ThrowableMeshComponent;

    FVector InitialVelocity;
};