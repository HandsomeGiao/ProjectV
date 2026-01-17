#include "ProjectV/Public/Player/V_AnimIns.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectV/ProjectV.h"

void UV_AnimIns::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (OwnerCharacter.IsValid())
        MovementComp = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
}

void UV_AnimIns::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // Calculate movement speed and direction
    if (MovementComp.IsValid())
    {
        Speed = MovementComp.IsValid() ? MovementComp->Velocity.Size2D() : 0.f;
        bIsInAir = MovementComp.IsValid() ? MovementComp->IsFalling() : false;
        if (OwnerCharacter.IsValid())
        {
            Velocity = MovementComp->Velocity;
            const FVector HorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0.f);
            Direction = UKismetAnimationLibrary::CalculateDirection(HorizontalVelocity,
                                                                    TryGetPawnOwner()->GetActorRotation());
        }
    }
}