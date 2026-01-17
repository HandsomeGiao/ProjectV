#include "Miscellaneous/ThrowableInstance/V_ThrowableInstance.h"

#include "Components/BoxComponent.h"
#include "ProjectV/ProjectV.h"

AV_ThrowableInstance::AV_ThrowableInstance()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    ThrowableMeshComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ThrowableMesh"));
    SetRootComponent(ThrowableMeshComponent);
    ThrowableMeshComponent->SetSimulatePhysics(true);
}

void AV_ThrowableInstance::SetInitialVelocity(const FVector& InitialSpeed)
{
    UE_LOG(LogProjectV, Log, TEXT("Set Initial Velocity: %s to ThrowableInstance %s"),
           *InitialSpeed.ToString(), *GetName());
    ThrowableMeshComponent->AddImpulse(InitialSpeed, NAME_None, true);
}

void AV_ThrowableInstance::BeginPlay()
{
    Super::BeginPlay();
    SetReplicateMovement(true);
}