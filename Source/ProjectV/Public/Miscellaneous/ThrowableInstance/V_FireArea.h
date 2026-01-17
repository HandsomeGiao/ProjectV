// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "V_FireArea.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UBoxComponent;

UCLASS()
class PROJECTV_API AV_FireArea : public AActor
{
    GENERATED_BODY()

public:
    AV_FireArea();

protected:
    // ========= 回调接口 =========
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex);

    // ====== override func =======
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ====== 配置变量 =======
    UPROPERTY(EditDefaultsOnly, Category="FireArea")
    TSubclassOf<UGameplayEffect> FireDamageGE;

    // ====== 内部变量 =======

    UPROPERTY()
    TMap<UAbilitySystemComponent*, FActiveGameplayEffectHandle> ActiveEffectMap;

    UPROPERTY(EditDefaultsOnly, Category="FireArea")
    UParticleSystemComponent* FireArc;

    UPROPERTY(EditDefaultsOnly, Category="FireArea")
    UBoxComponent* BoxComp;

    UPROPERTY(EditDefaultsOnly, Category="FireArea")
    float FireLifeSpan{5.f};

    FTimerHandle TimerHandle_LifeSpan;

    // ======== 内部方法 =======
    void TimerExpired();
    void RemoveAllEffects();
};