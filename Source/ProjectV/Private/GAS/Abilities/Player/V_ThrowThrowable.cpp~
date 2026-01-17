#include "GAS/Abilities/Player/V_ThrowThrowable.h"

#include "Miscellaneous/ThrowableInstance/V_ThrowableInstance.h"
#include "ProjectV/ProjectV.h"

void UV_ThrowThrowable::SpawnThrowable(TSubclassOf<AV_ThrowableInstance> ThrowableClass, FVector2D AimDirection,
                                       float ThrowSpeed)
{
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!IsValid(AvatarActor) || !AvatarActor->HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("SpawnThrowable: Invalid AvatarActor or not Authority in %s"), *GetName());
        return;
    }
    FVector SpawnLocation = AvatarActor->GetActorLocation() + AvatarActor->GetActorForwardVector() * 100.f +
                            FVector(0.f, 0.f, 50.f);
    if (!IsValid(ThrowableClass) || !GetWorld())
    {
        UE_LOG(LogProjectV, Warning, TEXT("SpawnThrowable: Invalid ThrowableClass Or Invalid World in %s"), *GetName());
        return;
    }
    AV_ThrowableInstance* ThrowIns = GetWorld()->SpawnActor<AV_ThrowableInstance>(
            ThrowableClass, SpawnLocation, FRotator::ZeroRotator,
            FActorSpawnParameters());
    if (!IsValid(ThrowIns))
    {
        UE_LOG(LogProjectV, Warning, TEXT("SpawnThrowable: Failed to spawn ThrowableInstance in %s"), *GetName());
        return;
    }
    // 将传入的AimDir向上抬起45度
    FVector Dir3D = FVector(AimDirection.X, AimDirection.Y, 0.0f);
    // 1. 找到右向量（用于绕其旋转，实现向上抬起）
    FVector RightVector = FVector::CrossProduct(Dir3D, FVector::UpVector);
    // 2. 创建旋转：绕着 RightVector 旋转 -45 度（UE 坐标系中负值通常代表抬起）
    FQuat UpQuat = FQuat(RightVector, FMath::DegreesToRadians(45.0f));
    // 3. 应用旋转
    FVector FinalDir = UpQuat.RotateVector(Dir3D);
    UE_LOG(LogProjectV, Log, TEXT("SpawnThrowable: Throw Direction: %s in %s"), *FinalDir.ToString(), *GetName());
    ThrowIns->SetInitialVelocity(FinalDir * ThrowSpeed);
}