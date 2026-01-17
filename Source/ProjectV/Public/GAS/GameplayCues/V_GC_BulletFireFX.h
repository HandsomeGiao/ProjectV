// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "V_GC_BulletFireFX.generated.h"

class AV_Bullet;

UCLASS()
class PROJECTV_API UV_GC_BulletFireFX : public UGameplayCueNotify_Static
{
    GENERATED_BODY()

protected:
    // ======= override func =======
    virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

    // ======= 配置变量 =======
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TObjectPtr<UParticleSystem> FireParticle;

    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TObjectPtr<USoundBase> FireSound;
};