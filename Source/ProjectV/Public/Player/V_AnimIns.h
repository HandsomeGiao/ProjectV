// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "V_AnimIns.generated.h"

class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class PROJECTV_API UV_AnimIns : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Speed = 0.f;
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsInAir = false;
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Direction = 0.f;
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    FVector Velocity = FVector::ZeroVector;

private:
    TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
    TWeakObjectPtr<UCharacterMovementComponent> MovementComp = nullptr;
};