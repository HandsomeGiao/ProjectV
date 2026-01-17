#include "Pickups/V_PickupsBase.h"

#include "Components/BoxComponent.h"
#include "ProjectV/ProjectV.h"

AV_PickupsBase::AV_PickupsBase()
{
    bReplicates = true;

    PrimaryActorTick.bCanEverTick = false;

    // 在编辑器中配置碰撞通道
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupCollision"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
    CollisionComponent->SetGenerateOverlapEvents(true);
    RootComponent = CollisionComponent;
}

// 这是在外部进行调用的，可以看到CollisionComponent没有绑定Overlap事件
void AV_PickupsBase::PickedUpBy(AActor* Actor)
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("PickedUpBy called on client in %s"), *GetName());
        return;
    }

    UE_LOG(LogProjectV, Log, TEXT("%s picked up by %s"), *GetName(), *Actor->GetName());
    if (CollisionComponent)
    {
        CollisionComponent->SetSimulatePhysics(false);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AV_PickupsBase::Thrownup()
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("Thrownup called on client in %s"), *GetName());
        return;
    }

    CollisionComponent->SetSimulatePhysics(true);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
    // 如果不增加此延时定时器，直接开启碰撞，往人身上丢时会导致奇怪的BUG，暂不明白原因。
    GetWorld()->GetTimerManager().SetTimer(SetPysTimer, [this]()
    {
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }, 3.f, false);
}

void AV_PickupsBase::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
        SetReplicateMovement(true);
}

void AV_PickupsBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (GetWorldTimerManager().TimerExists(SetPysTimer))
    {
        GetWorldTimerManager().ClearTimer(SetPysTimer);
    }
}