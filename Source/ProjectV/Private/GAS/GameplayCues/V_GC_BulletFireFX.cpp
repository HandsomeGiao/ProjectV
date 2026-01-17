// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/V_GC_BulletFireFX.h"

#include "Kismet/GameplayStatics.h"

bool UV_GC_BulletFireFX::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
    FVector SpawnLocation = Parameters.Location;
    FRotator SpawnRotation = Parameters.Normal.Rotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UGameplayStatics::SpawnEmitterAtLocation(MyTarget, FireParticle, SpawnLocation, SpawnRotation);
    UGameplayStatics::PlaySoundAtLocation(MyTarget, FireSound, SpawnLocation, SpawnRotation);

    return true;
}