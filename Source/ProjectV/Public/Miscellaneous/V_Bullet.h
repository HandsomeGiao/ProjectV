// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "V_Bullet.generated.h"

class USphereComponent;
class UGameplayEffect;
class UProjectileMovementComponent;

UCLASS()
class PROJECTV_API AV_Bullet : public AActor
{
    GENERATED_BODY()

public:
    AV_Bullet();

protected:
    // ======= 内部方法 ========
    void OnDestroyTimerExpired();
    
    UFUNCTION()
    void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
                         OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // ======= override func ========
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

    // ======= 内部变量 ========

    FTimerHandle DestroyTimerHandle;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMeshComponent> BulletMesh;
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USphereComponent> CollisionComponent;
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    // ======= 配置变量 ========

    UPROPERTY(EditDefaultsOnly, Category="Bullet")
    TSubclassOf<UGameplayEffect> DamageEffectClass;
    UPROPERTY(EditDefaultsOnly, Category="Bullet")
    USoundBase* BulletHitSound;
    UPROPERTY(EditDefaultsOnly, Category="Bullet")
    UParticleSystem* BulletHitParticle;
};