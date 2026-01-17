// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "V_ThrowableInstance.h"
#include "V_Instance_Explosive.generated.h"

UCLASS()
class PROJECTV_API AV_Instance_Explosive : public AV_ThrowableInstance
{
    GENERATED_BODY()

public:
    AV_Instance_Explosive();

protected:
    // ====== override func =======
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ====== 内部变量 =======
    
    FTimerHandle DestroyTimerHandle;

    // ====== 配置变量 =======
    
    UPROPERTY(EditDefaultsOnly, Category="Throwable")
    UParticleSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, Category="Throwable")
    float ExplosionRadius{300.f};

    UPROPERTY(EditDefaultsOnly, Category="Throwable")
    float ExplosionDamage{50.f};

    UPROPERTY(EditDefaultsOnly, Category="Throwable")
    TSubclassOf<UGameplayEffect> ExplosionDamageEffectClass;
    
    // ====== 内部方法 =======
    
    void OnTimerExpired();
};