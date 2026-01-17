#include "Pickups/Weapons/V_WeaponBase.h"
#include "AbilitySystemComponent.h"
#include "VGameplayTags.h"
#include "GAS/ASC/V_WeaponASC.h"
#include "GAS/AttributeSet/V_WeaponAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Player/V_PlayerCharacter.h"
#include "ProjectV/ProjectV.h"

AV_WeaponBase::AV_WeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    WeaponMesh->SetupAttachment(RootComponent);

    WeaponASC = CreateDefaultSubobject<UV_WeaponASC>(TEXT("WeaponAbilitySystemComponent"));
    WeaponASC->SetIsReplicated(true);
    // just replicate Tags and Attributes
    WeaponASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    WeaponAttributeSet = CreateDefaultSubobject<UV_WeaponAttributeSet>(TEXT("WeaponAttributeSet"));
}

void AV_WeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, OwningPlayer);
}

// 被丢到地上
void AV_WeaponBase::Thrownup()
{
    // call on server
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("Unequipped called on client in %s"), *GetName());
        return;
    }
    Super::Thrownup();
    OwningPlayer = nullptr;
    HideSelf(false);

    // 如果正在换弹，则需要取消换弹
}

void AV_WeaponBase::Unequipped()
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("Unequipped called on server in %s"), *GetName());
        return;
    }

    HideSelf(true);

    // cancel reloading ability
    FGameplayTagContainer ReloadTag;
    ReloadTag.AddTag(VTags::VAbilites::VWeapons::WeaponReload);
    WeaponASC->CancelAbilities(&ReloadTag);

    // Optimize: Unequipped后未清空OwningPlayer,会导致Transform继续同步，浪费带宽。
}

void AV_WeaponBase::Equipped()
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("PickedUpBy called on client in %s"), *GetName());
        return;
    }

    HideSelf(false);
}

void AV_WeaponBase::PickedUpBy(AActor* InActor)
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("PickedUpBy called on client in %s"), *GetName());
        return;
    }

    Super::PickedUpBy(InActor);

    // weapon只能被player捡起
    OwningPlayer = CastChecked<AV_PlayerCharacter>(InActor);
}

FTransform AV_WeaponBase::GetMuzzleTransform() const
{
    // 得到WeaponMesh的MuzzleFlash socket位置
    FName MuzzleSocketName = FName("MuzzleFlash");
    if (!WeaponMesh->DoesSocketExist(MuzzleSocketName))
        return GetActorTransform();
    FTransform MuzzleTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
    return MuzzleTransform;
}

void AV_WeaponBase::BeginPlay()
{
    Super::BeginPlay();

    check(IsValid(WeaponASC));
    // Give Starting Abilities and effect
    if (HasAuthority())
    {
        WeaponASC->InitAbilityActorInfo(this, this);
        // effect
        if (IsValid(InitEffectClass))
        {
            FGameplayEffectContextHandle EffectContext = WeaponASC->MakeEffectContext();
            FGameplayEffectSpecHandle SpecHandle = WeaponASC->MakeOutgoingSpec(InitEffectClass, 1, EffectContext);
            check(SpecHandle.IsValid());
            WeaponASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }

        // abilities
        for (TSubclassOf<UGameplayAbility> AbilityClass : StartingAbilities)
        {
            if (!IsValid(AbilityClass))
            {
                UE_LOG(LogProjectV, Warning, TEXT("Ability class in Weapon %s is invalid"), *GetName());
                continue;
            }

            auto Handle = WeaponASC->GiveAbility(AbilityClass);
            UE_LOG(LogProjectV, Log, TEXT("Given Ability %s to Weapon %s, Handle: %s"),
                   *AbilityClass->GetName(),
                   *GetName(),
                   *Handle.ToString());
        }
    }
}

void AV_WeaponBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // reset spread angle
    SpreadAngle = FMath::Max(SpreadAngle - SpreadAngleResetSpeed * DeltaTime, MinSpreadAngle);
    SpreadAngle = FMath::Min(SpreadAngle, MaxSpreadAngle);

    // Optimize: 每帧同步Transform，开销较大。
    if (OwningPlayer.IsValid())
    {
        FTransform WeaponTransform = OwningPlayer->GetFollowLocation();
        SetActorTransform(WeaponTransform);

        // 被玩家装备的时候才画Debug锥体
        if (!IsHidden() && bShowDebugCone && HasAuthority())
        {
            //debug
            FTransform MuzzleTf = GetMuzzleTransform();
            float HalfRad = FMath::DegreesToRadians(SpreadAngle / 2.f);
            DrawDebugCone(GetWorld(), MuzzleTf.GetLocation(), MuzzleTf.GetRotation().GetForwardVector(), DebugConeDis,
                          HalfRad,
                          HalfRad, 12, FColor::Red);
        }
    }
    //UE_LOG(LogProjectV, Display, TEXT("Weapon %s , New Transform: %s"), *GetName(), *WeaponTransform.ToString());
}

void AV_WeaponBase::AddSpreadAngle()
{
    SpreadAngle += SpreadAnglePerShot;
}

UAbilitySystemComponent* AV_WeaponBase::GetAbilitySystemComponent() const
{
    return WeaponASC;
}

bool AV_WeaponBase::IsAmmoFull()
{
    return WeaponASC->GetNumericAttribute(WeaponAttributeSet->GetCurrentAmmoAttribute()) >=
           WeaponASC->GetNumericAttribute(WeaponAttributeSet->GetMaxAmmoAttribute());
}

void AV_WeaponBase::HideSelf(bool bHide)
{
    // called on server
    if (bHide)
    {
        SetActorHiddenInGame(true);
    }
    else
    {
        SetActorHiddenInGame(false);
    }
}