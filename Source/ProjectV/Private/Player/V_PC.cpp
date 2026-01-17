// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectV/Public/Player/V_PC.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "VGameplayTags.h"
#include "Inventory/Inv_InventoryBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pickups/Weapons/V_WeaponBase.h"
#include "Player/V_PlayerCharacter.h"
#include "Player/V_PlayerState.h"
#include "ProjectV/ProjectV.h"


class UInv_InventoryBase;

AV_PC::AV_PC()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AV_PC::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 检测可拾取物品
    FVector CameraLocation;
    FRotator CameraRotation;
    GetPlayerViewPoint(CameraLocation, CameraRotation);
    if (IsValid(PlayerChar.Get()))
    {
        FHitResult HitResult;
        UKismetSystemLibrary::BoxTraceSingle(this, PlayerChar->GetActorLocation(),
                                             PlayerChar->GetActorLocation() + FVector(0, 0, 50),
                                             FVector(50, 50, 50), CameraRotation,
                                             UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
                                             TArray<AActor*>{PlayerChar.Get()}, EDrawDebugTrace::ForOneFrame,
                                             HitResult, true, FColor::Red, FColor::Green, 5.f);
        DetectedItemActor = HitResult.GetActor();
    }
}


void AV_PC::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
            else
            {
                UE_LOG(LogProjectV, Warning, TEXT("DefaultMappingContext is not set in %s"), *GetName());
            }
        }
    }
    bShowMouseCursor = true;
}

void AV_PC::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (MoveAction)
        {
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AV_PC::Move);
        }

        if (JumpAction)
        {
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AV_PC::JumpStarted);
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AV_PC::JumpCompleted);
        }

        if (CrouchAction)
        {
            EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &AV_PC::CrouchStarted);
            EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AV_PC::CrouchCompleted);
        }
        if (PrimaryAttackAction)
        {
            EnhancedInput->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &AV_PC::PrimaryAttackBegin);
            EnhancedInput->BindAction(PrimaryAttackAction, ETriggerEvent::Completed, this, &AV_PC::PrimaryAttackEnd);
        }
        if (ReloadAction)
        {
            EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &AV_PC::Reload);
        }
        if (EquipWeapon0Action)
        {
            EnhancedInput->BindAction(EquipWeapon0Action, ETriggerEvent::Started, this, &AV_PC::EquipWeapon0);
        }
        if (EquipWeapon1Action)
        {
            EnhancedInput->BindAction(EquipWeapon1Action, ETriggerEvent::Started, this, &AV_PC::EquipWeapon1);
        }
        if (ThrowupCurrentWeaponAction)
        {
            EnhancedInput->BindAction(
                    ThrowupCurrentWeaponAction, ETriggerEvent::Started, this, &AV_PC::ThrowupCurrentWeapon);
        }
        if (ThrowThrowableAction)
        {
            EnhancedInput->BindAction(ThrowThrowableAction, ETriggerEvent::Started, this, &AV_PC::ThrowThrowable);
            EnhancedInput->BindAction(ThrowThrowableAction, ETriggerEvent::Completed, this, &AV_PC::EndThrowThrowable);
        }

        if (SelectInventoryItemAction)
        {
            EnhancedInput->BindAction(SelectInventoryItemAction, ETriggerEvent::Triggered, this,
                                      &AV_PC::SelectInventorySlot);
        }
        if (PickupItemAction)
        {
            EnhancedInput->BindAction(PickupItemAction, ETriggerEvent::Started, this, &AV_PC::PickupItem);
        }
    }
}

void AV_PC::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    PlayerChar = CastChecked<AV_PlayerCharacter>(InPawn);
}

void AV_PC::OnRep_Pawn()
{
    Super::OnRep_Pawn();

    PlayerChar = CastChecked<AV_PlayerCharacter>(GetPawn());
}

void AV_PC::Move(const FInputActionValue& Value)
{
    const FVector2D MoveVector = Value.Get<FVector2D>();
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    const FVector Forward = FVector::ForwardVector;
    const FVector Right = FVector::RightVector;

    ControlledPawn->AddMovementInput(Forward, MoveVector.Y);
    ControlledPawn->AddMovementInput(Right, MoveVector.X);
}

void AV_PC::JumpStarted()
{
    if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
    {
        Char->Jump();
    }
}

void AV_PC::JumpCompleted()
{
    if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
    {
        Char->StopJumping();
    }
}

void AV_PC::CrouchStarted()
{
    if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
    {
        Char->Crouch();
    }
}

void AV_PC::CrouchCompleted()
{
    if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
    {
        Char->UnCrouch();
    }
}

void AV_PC::PrimaryAttackBegin(const FInputActionValue& Value)
{
    UE_LOG(LogProjectV, Log, TEXT("Primary Attack triggered in %s"), *GetName());
    if (!IsValid(PlayerState))
    {
        UE_LOG(LogProjectV, Warning, TEXT("PrimaryAttack: PlayerState is not valid in %s"), *GetName());
        return;
    }

    if (!bIsThrowing)
    {
        // primary attack
        auto ASC = CastChecked<AV_PlayerState>(PlayerState)->GetAbilitySystemComponent();
        if (IsValid(ASC))
        {
            FGameplayTagContainer GTags;
            GTags.AddTag(VTags::VAbilites::Player::PlayerPrimaryAttack);
            if (ASC->AbilityActorInfo.IsValid())
                ASC->TryActivateAbilitiesByTag(GTags);
            else
            {
                UE_LOG(LogProjectV, Warning, TEXT("PrimaryAttack: AbilityActorInfo is not valid in %s"), *GetName());
            }
        }
    }
    else
    {
        // compute direction
        FVector2D AimDir = FVector2D::UnitVector;
        if (FHitResult HitResult; GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            FVector MouseLocation = HitResult.Location;
            FVector PawnLocation = GetPawn()->GetActorLocation();

            FVector Direction = MouseLocation - PawnLocation;
            AimDir = FVector2D(Direction.X, Direction.Y).GetSafeNormal();
        }

        Server_ConfirmThrow(AimDir);
    }
}

void AV_PC::PrimaryAttackEnd()
{
    UE_LOG(LogProjectV, Log, TEXT("Primary Attack ended in %s"), *GetName());
    // Gameplay Event不会网络同步
    Server_PrimaryAttackEnd();
}

void AV_PC::Server_PrimaryAttackEnd_Implementation()
{
    if (PlayerChar.IsValid())
    {
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
                PlayerChar->GetCurrentWeapon(), VTags::VAbilites::Player::PlayerPrimaryAttackEnd,
                FGameplayEventData());
        UE_LOG(LogProjectV, Log, TEXT("Sent PrimaryAttackEnd event to weapon in %s"), *GetName());
    }
}

void AV_PC::Reload()
{
    UE_LOG(LogProjectV, Log, TEXT("Reload triggered in %s"), *GetName());
    if (!IsValid(PlayerState))
    {
        UE_LOG(LogProjectV, Warning, TEXT("PrimaryAttack: PlayerState is not valid in %s"), *GetName());
        return;
    }
    auto ASC = CastChecked<AV_PlayerState>(PlayerState)->GetAbilitySystemComponent();

    // 检查是否还剩余弹药
    if (PlayerChar.IsValid() && (PlayerChar->GetReserveAmmo() <= 0.f || PlayerChar->IsCurrentWeaponAmmoFull()))
    {
        UE_LOG(LogProjectV, Log, TEXT("Reload: No reserve ammo in %s"), *GetName());
        return;
    }

    FGameplayTagContainer GTags;
    GTags.AddTag(VTags::VAbilites::Player::PlayerReload);
    if (ASC->AbilityActorInfo.IsValid())
        ASC->TryActivateAbilitiesByTag(GTags);
    else
    {
        UE_LOG(LogProjectV, Warning, TEXT("PrimaryAttack: AbilityActorInfo is not valid in %s"), *GetName());
    }
}

void AV_PC::EquipWeapon0()
{
    if (PlayerChar.IsValid())
        PlayerChar->EquipWeapon(0);
}

void AV_PC::EquipWeapon1()
{
    if (PlayerChar.IsValid())
        PlayerChar->EquipWeapon(1);
}

void AV_PC::ThrowupCurrentWeapon()
{
    if (PlayerChar.IsValid())
        PlayerChar->ThrowupCurrentWeapon();
}

void AV_PC::ThrowThrowable()
{
    UE_LOG(LogProjectV, Log, TEXT("ThrowAction triggered in %s"), *GetName());
    if (!IsValid(PlayerState))
    {
        UE_LOG(LogProjectV, Warning, TEXT("ThrowAction: PlayerState is not valid in %s"), *GetName());
        return;
    }
    auto ASC = CastChecked<AV_PlayerState>(PlayerState)->GetAbilitySystemComponent();

    FGameplayTagContainer GTags;
    GTags.AddTag(VTags::VAbilites::Player::PlayerThrowThrowable);
    ASC->TryActivateAbilitiesByTag(GTags);
    bIsThrowing = true;
}

void AV_PC::EndThrowThrowable()
{
    UE_LOG(LogProjectV, Log, TEXT("ThrowAction triggered in %s"), *GetName());
    if (!IsValid(PlayerState))
    {
        UE_LOG(LogProjectV, Warning, TEXT("ThrowAction: PlayerState is not valid in %s"), *GetName());
        return;
    }
    auto ASC = CastChecked<AV_PlayerState>(PlayerState)->GetAbilitySystemComponent();

    FGameplayTagContainer GTags;
    GTags.AddTag(VTags::VAbilites::Player::PlayerThrowThrowable);
    ASC->CancelAbilities(&GTags);
    bIsThrowing = false;
}

void AV_PC::SelectInventorySlot(const FInputActionValue& Value)
{
    // 在Fragments中定义了UsedFragments和SelectEffectFragments，分别实现被选中时和被使用时的效果,
    // 但在这里不用担心，调用InventoryBase的对应函数就行。
    if (!PlayerChar.IsValid())
    {
        UE_LOG(LogProjectV, Warning, TEXT("SelectInventorySlot: PlayerChar is not valid in %s"), *GetName());
        return;
    }
    SetInventoryBase();
    if (!IsValid(InventoryBase.Get()))
    {
        UE_LOG(LogProjectV, Warning, TEXT("SelectInventorySlot: InventoryBase is not valid in %s"), *GetName());
        return;
    }
    int32 SlotIndex = static_cast<int32>(Value.Get<float>() - 1); // Inventory slot is 0-based
    InventoryBase->SelectItem(SlotIndex);
    UE_LOG(LogProjectV, Log,
           TEXT("SelectInventorySlot: Selected slot %d in %s"), SlotIndex, *GetName());
}

void AV_PC::UseItem()
{
    if (!PlayerChar.IsValid())
    {
        UE_LOG(LogProjectV, Warning, TEXT("SelectInventorySlot: PlayerChar is not valid in %s"), *GetName());
        return;
    }
    SetInventoryBase();
    InventoryBase = PlayerChar->FindComponentByClass<UInv_InventoryBase>();
    if (!IsValid(InventoryBase.Get()))
    {
        UE_LOG(LogProjectV, Warning, TEXT("SelectInventorySlot: InventoryBase is not valid in %s"), *GetName());
        return;
    }
    InventoryBase->UseItem();
}

void AV_PC::PickupItem()
{
    SetInventoryBase();
    if (!InventoryBase.IsValid())
    {
        UE_LOG(LogProjectV, Warning, TEXT("PickupItem: InventoryBase is not valid in %s"), *GetName());
        return;
    }
    if (!DetectedItemActor.IsValid())
    {
        UE_LOG(LogProjectV, Log, TEXT("PickupItem: No detected item to pick up in %s"), *GetName());
        return;
    }
    InventoryBase->ServerPickupItem(DetectedItemActor.Get());
}

void AV_PC::SetInventoryBase()
{
    if (InventoryBase.IsValid())
        return;
    InventoryBase = PlayerChar->FindComponentByClass<UInv_InventoryBase>();
}

void AV_PC::Server_ConfirmThrow_Implementation(FVector2D AimDir)
{
    FGameplayEventData EventData;
    EventData.EventTag = VTags::Events::Player::PlayerConfirmThrowThrowable;
    UV_PlayerConfirmThrowData* OptionalObj = NewObject<UV_PlayerConfirmThrowData>();
    EventData.OptionalObject = OptionalObj;
    OptionalObj->AimDirection = AimDir;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            GetPawn(), VTags::Events::Player::PlayerConfirmThrowThrowable, EventData);
    UE_LOG(LogProjectV, Log, TEXT("Sent ConfirmThrow event to player in %s"), *GetName());
}