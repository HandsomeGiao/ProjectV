// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Weapons/V_GA_WeaponFire.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Miscellaneous/V_Bullet.h"
#include "Pickups/Weapons/V_WeaponBase.h"

// called on server
void UV_GA_WeaponFire::SpawnFx(int32 NumBullets)
{
    // Spawn Bullet Gameplay Cue at Muzzle Location
    AV_WeaponBase* Weapon = Cast<AV_WeaponBase>(GetOwningActorFromActorInfo());
    checkf(IsValid(Weapon), TEXT("V_GA_WeaponFire::SpawnBullet: Owning Actor is not a Weapon!"));
    FTransform MuzzleTransform = Weapon->GetMuzzleTransform();
    FGameplayCueParameters Params;
    Params.Location = MuzzleTransform.GetLocation();
    Params.Normal = MuzzleTransform.GetRotation().GetForwardVector();
    GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
            FireGCTag, Params);
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    // Optimize: 生成的子弹是replicated的，在服务器上生成，并且在服务器上进行摧毁
    // 服务器检测到碰撞时传递销毁的信息给客户端，会导致一定的网络延迟

    float HalfRad = FMath::DegreesToRadians(Weapon->GetSpreadAngle() / 2.f);
    while (NumBullets-- > 0)
    {
        // 确保子弹不会相互碰撞
        const FVector ShootDir = FMath::VRandCone(MuzzleTransform.GetRotation().GetForwardVector(), HalfRad);
        Weapon->GetWorld()->SpawnActor<AV_Bullet>(BulletClass, MuzzleTransform.GetLocation(),
                                                  ShootDir.Rotation(), SpawnParams);
    }
    // UE_LOG(LogProjectV, Warning, TEXT("Spawning Bullet at Transform: %s, in %s"),
    //        *MuzzleTransform.ToString(),
    //        AvatarActor->HasAuthority()? TEXT("Server") : TEXT("Client"));
}