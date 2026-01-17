#include "ProjectV/Public/Player/V_PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "VGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/ASC/V_PlayerASC.h"
#include "GAS/ASC/V_WeaponASC.h"
#include "GAS/AttributeSet/V_PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Pickups/Weapons/V_WeaponBase.h"
#include "Player/V_PlayerHUD.h"
#include "Player/V_PlayerState.h"
#include "ProjectV/ProjectV.h"

AV_PlayerCharacter::AV_PlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true);

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->GravityScale = 1.0f;
    MoveComp->MaxAcceleration = 2400.0f;
    MoveComp->BrakingFrictionFactor = 1.0f;
    MoveComp->BrakingFriction = 6.0f;
    MoveComp->GroundFriction = 8.0f;
    MoveComp->BrakingDecelerationWalking = 1400.0f;
    MoveComp->bUseControllerDesiredRotation = false;
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    MoveComp->SetCrouchedHalfHeight(65.0f);
    MoveComp->SetIsReplicated(true);
}

UAbilitySystemComponent* AV_PlayerCharacter::GetAbilitySystemComponent() const
{
    AV_PlayerState* PS = GetPlayerState<AV_PlayerState>();
    return IsValid(PS) ? PS->GetAbilitySystemComponent() : nullptr;
}

void AV_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AV_PlayerCharacter, WeaponTransform, COND_SkipOwner);
    DOREPLIFETIME(AV_PlayerCharacter, CurrentWeaponIndex);
    DOREPLIFETIME(AV_PlayerCharacter, Weapon0);
    DOREPLIFETIME(AV_PlayerCharacter, Weapon1);
}

// 装备后的武器必须有效（即不能切换到空手状态）
void AV_PlayerCharacter::EquipWeapon(int8 WeaponIndex)
{
    // call on client
    if (WeaponIndex == CurrentWeaponIndex)
        return;
    Server_EquipWeapon(WeaponIndex);
}

void AV_PlayerCharacter::ThrowupCurrentWeapon()
{
    if (!IsValid(GetCurrentWeapon()))
    {
        UE_LOG(LogProjectV, Log, TEXT("ThrowupCurrentWeapon: No CurrentWeapon in %s"), *GetName());
        return;
    }
    Server_ThrowupCurrentWeapon();
}

#pragma warning(push)
#pragma warning(disable: 4715) // 禁用 C4715
// ReSharper disable once CppNotAllPathsReturnValue
FOnGameplayAttributeValueChange& AV_PlayerCharacter::GetAmmoChangeDelegate()
{
    check(IsValid(GetCurrentWeapon()));
    switch (GetCurrentWeapon()->GetWeaponType())
    {
        case EWeaponType::Pistol:
            return PlayerASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetPistolAmmoAttribute());
        case EWeaponType::Rifle:
            return PlayerASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetRifleAmmoAttribute());
        case EWeaponType::Shotgun:
            return PlayerASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetShotgunAmmoAttribute());
        default:
            // won't reach here
            checkf(false, TEXT("Unknown weapon type in GetAmmoChangeDelegate in %s"), *GetName());
    }
}
#pragma warning(pop)

float AV_PlayerCharacter::GetReserveAmmo()
{
    if (!PlayerASC.IsValid() || !IsValid(GetCurrentWeapon()))
        return 0.f;

    switch (GetCurrentWeapon()->GetWeaponType())
    {
        case EWeaponType::Pistol:
            return PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetPistolAmmoAttribute());
        case EWeaponType::Rifle:
            return PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetRifleAmmoAttribute());
        case EWeaponType::Shotgun:
            return PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetShotgunAmmoAttribute());
        default:
            UE_LOG(LogProjectV, Warning, TEXT("Unknown weapon type in GetCurrentAmmo in %s"), *GetName());
            return 0.f;
    }
}

bool AV_PlayerCharacter::IsCurrentWeaponAmmoFull()
{
    if (!IsValid(GetCurrentWeapon()))
    {
        UE_LOG(LogProjectV, Warning, TEXT("IsCurrentWeaponAmmoFull: No CurrentWeapon in %s"), *GetName());
        return false;
    }
    return GetCurrentWeapon()->IsAmmoFull();
}

void AV_PlayerCharacter::Server_GiveGA_Implementation(TSubclassOf<UGameplayAbility> AbilityClass)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!IsValid(ASC))
    {
        UE_LOG(LogProjectV, Warning, TEXT("AbilitySystemComponent is not valid in %s"), *GetName());
        return;
    }
    ASC->GiveAbility(AbilityClass);
}

void AV_PlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UCapsuleComponent* Capsule = GetCapsuleComponent(); IsValid(Capsule) && HasAuthority())
    {
        // only server handles pickup overlap
        Capsule->OnComponentBeginOverlap.AddDynamic(this, &AV_PlayerCharacter::OnPickupOverlap);
    }
}

void AV_PlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (IsLocallyControlled())
    {
        // only calculate weapon location on local client
        CalculateWeaponLocation(DeltaSeconds);
        LastCharacterLocation = GetActorLocation();
    }
}

void AV_PlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // this func only called in server
    PlayerASC = CastChecked<UV_PlayerASC>(GetAbilitySystemComponent());
    check(PlayerASC.IsValid());
    PlayerASC->InitAbilityActorInfo(GetPlayerState(), this);
    GiveStartupAbilities();
    InitializeAttributes();
    // init walk speed
    InitMoveSpeedFromASC();

    // listen server
    if (NewController->IsLocalPlayerController())
    {
        APlayerController* PC = Cast<APlayerController>(NewController);
        PlayerHUD = Cast<AV_PlayerHUD>(PC->GetHUD());
        check(IsValid(PlayerHUD.Get()));
        // init UI
        PlayerHUD->InitLayout(this);
        PlayerHUD->SetMaxHealth(PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetMaxHealthAttribute()));
        PlayerHUD->SetHealth(PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetHealthAttribute()));
        // dont init ammo here, wait for weapon equip
    }
}

void AV_PlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    PlayerASC = CastChecked<UV_PlayerASC>(GetAbilitySystemComponent());
    check(PlayerASC.IsValid());

    // 客户端上不需要初始化这个，这OwnerActor和AvatarActor是replicated的
    // ASC->InitAbilityActorInfo(GetPlayerState(), this);

    if (PlayerHUD.IsValid())
    {
        PlayerHUD->InitLayout(this);
        //UE_LOG(LogProjectV, Log, TEXT("OnRep_PlayerState: Initialized HUD in %s"), *GetName());
        InitMoveSpeedFromASC();
    }
}

void AV_PlayerCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();

    APlayerController* PC = Cast<APlayerController>(Controller);
    PlayerHUD = Cast<AV_PlayerHUD>(PC->GetHUD());
    check(IsValid(PlayerHUD.Get()));
    // init UI

    if (PlayerASC.IsValid())
    {
        PlayerHUD->InitLayout(this);
        //UE_LOG(LogProjectV, Log, TEXT("OnRep_Controller: Initialized HUD in %s"), *GetName());
        InitMoveSpeedFromASC();
    }
}

void AV_PlayerCharacter::InitMoveSpeedFromASC()
{
    UE_LOG(LogProjectV, Log, TEXT("InitMoveSpeedFromASC called in %s"), *GetName());
    if (!IsValid(PlayerASC.Get()))
    {
        UE_LOG(LogProjectV, Warning, TEXT("InitMoveSpeedFromASC: PlayerASC is null in %s"), *GetName());
        return;
    }

    // avoid duplicate binding
    if (MoveSpeedDlgtHandle.IsValid())
    {
        PlayerASC->GetGameplayAttributeValueChangeDelegate(
                UV_PlayerAttributeSet::GetMoveSpeedAttribute()).Remove(MoveSpeedDlgtHandle);
    }

    MoveSpeedDlgtHandle = PlayerASC->GetGameplayAttributeValueChangeDelegate(
            UV_PlayerAttributeSet::GetMoveSpeedAttribute()).AddLambda(
            [this](const FOnAttributeChangeData& Data)
            {
                float NewMoveSpeed = Data.NewValue;
                UCharacterMovementComponent* MoveComp = GetCharacterMovement();
                if (IsValid(MoveComp))
                {
                    MoveComp->MaxWalkSpeed = NewMoveSpeed;
                    UE_LOG(LogProjectV, Log,
                           TEXT("MoveSpeed changed to %f in %s"), NewMoveSpeed, *GetName());
                }
            });
    GetCharacterMovement()->MaxWalkSpeed = PlayerASC->GetNumericAttribute(
            UV_PlayerAttributeSet::GetMoveSpeedAttribute());
    UE_LOG(LogProjectV, Log,
           TEXT("InitMoveSpeedFromASC: MoveSpeed initialized to %f in %s"),
           GetCharacterMovement()->MaxWalkSpeed,
           *GetName());
}

void AV_PlayerCharacter::OnPickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult)
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("OnPickupOverlap called on client in %s"), *GetName());
        return;
    }

    if (!OtherActor || OtherActor == this)
    {
        UE_LOG(LogProjectV, Warning, TEXT("OnPickupOverlap: OtherActor is null or self in %s"), *GetName());
        return;
    }

    // 尝试捡起武器
    if (AV_WeaponBase* Weapon = Cast<AV_WeaponBase>(OtherActor))
    {
        // 可以捡起武器
        if (!IsValid(Weapon0) || !IsValid(Weapon1))
        {
            Weapon->PickedUpBy(this);
            Weapon->Unequipped();
            // 有空位，直接捡起，注意，空手时，CurrentWeaponIndex为-1
            // 不存在捡起后直接装备的逻辑
            if (!IsValid(Weapon0))
            {
                Weapon0 = Weapon;
                if (PlayerHUD.IsValid())
                    PlayerHUD->SetWeapon0Icon(Weapon->GetAmmoIcon());
            }
            else
            {
                Weapon1 = Weapon;
                if (PlayerHUD.IsValid())
                    PlayerHUD->SetWeapon1Icon(Weapon->GetAmmoIcon());
            }
        }
    }
}

void AV_PlayerCharacter::UpdateAmmoUI()
{
    if (PlayerHUD.IsValid() && PlayerASC.IsValid())
    {
        AV_WeaponBase* CurrentWeapon = nullptr;
        if (CurrentWeaponIndex == static_cast<int8>(0))
            CurrentWeapon = Weapon0;
        else if (CurrentWeaponIndex == static_cast<int8>(1))
            CurrentWeapon = Weapon1;

        if (IsValid(CurrentWeapon))
        {
            switch (CurrentWeapon->GetWeaponType())
            {
                case EWeaponType::Pistol:
                    PlayerHUD->SetReserveAmmo(
                            PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetMaxPistolAmmoAttribute()));
                    PlayerHUD->SetCurrentAmmo(
                            PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetPistolAmmoAttribute()));
                    break;
                case EWeaponType::Rifle:
                    PlayerHUD->SetReserveAmmo(
                            PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetMaxRifleAmmoAttribute()));
                    PlayerHUD->SetCurrentAmmo(
                            PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetRifleAmmoAttribute()));
                    break;
                case EWeaponType::Shotgun:
                    PlayerHUD->SetReserveAmmo(
                            PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetMaxShotgunAmmoAttribute()));
                    PlayerHUD->SetCurrentAmmo(
                            PlayerASC->GetNumericAttribute(UV_PlayerAttributeSet::GetShotgunAmmoAttribute()));
                    break;
                default:
                    UE_LOG(LogProjectV, Warning, TEXT("Unknown weapon type in UpdateAmmoUI in %s"), *GetName());
                    break;
            }
        }
        else
        {
            // unarmed
            PlayerHUD->SetReserveAmmo(0);
            PlayerHUD->SetCurrentAmmo(0);
        }
    }
}

void AV_PlayerCharacter::OnRep_CurrentWeaponIndex()
{
    // update ui
    BindWeaponUI();
}

void AV_PlayerCharacter::OnRep_Weapon0()
{
    // 为了避免CurrentWeaponIndex和对应WeaponActor*的复制时序问题
    // 例如玩家一直试图切换到武器1，然后捡起武器1，如果CurrentWeaponIndex先复制了，那么UI会绑定失败
    if (CurrentWeaponIndex == static_cast<int8>(0))
    {
        // Optimize: 这里应该要避免重复绑定吗？但是也没有太大问题
        BindWeaponUI();
    }
    if (PlayerHUD.IsValid())
        PlayerHUD->SetWeapon0Icon(IsValid(Weapon0) ? Weapon0->GetAmmoIcon() : nullptr);
}

void AV_PlayerCharacter::OnRep_Weapon1()
{
    // 为了避免CurrentWeaponIndex和对应WeaponActor*的复制时序问题
    // 例如玩家一直试图切换到武器1，然后捡起武器1，如果CurrentWeaponIndex先复制了，那么UI会绑定失败
    if (CurrentWeaponIndex == static_cast<int8>(1))
    {
        // Optimize: 这里应该要避免重复绑定吗？但是也没有太大问题
        BindWeaponUI();
    }
    if (PlayerHUD.IsValid())
        PlayerHUD->SetWeapon1Icon(IsValid(Weapon1) ? Weapon1->GetAmmoIcon() : nullptr);
}

void AV_PlayerCharacter::BindWeaponUI()
{
    AV_WeaponBase* Weapon = GetCurrentWeapon();
    if (!PlayerHUD.IsValid())
    {
        UE_LOG(LogProjectV, Log, TEXT("BindWeaponUI: PlayerHUD is null in %s"), *GetName());
        return;
    }
    PlayerHUD->SetWeapon(Weapon);
}

void AV_PlayerCharacter::Server_UpdateWeaponTransform_Implementation(const FTransform& NewTransform)
{
    WeaponTransform = NewTransform;
}

void AV_PlayerCharacter::CalculateWeaponLocation(float DeltaTime)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    FHitResult HitResult;
    if (PC && PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult))
    {
        FVector MouseLocation = HitResult.Location;
        FVector CharacterLocation = GetActorLocation();
        MouseLocation.Z = CharacterLocation.Z;
        const FVector Direction = (MouseLocation - CharacterLocation).GetSafeNormal();
        const FVector TargetLoc = CharacterLocation + Direction * WeaponDis;
        // 将武器位置插值到目标位置，避免瞬移注意，武器位置必须环绕人物周围一圈移动
        const FVector InterpLoc = FMath::VInterpTo(WeaponTransform.GetLocation(), TargetLoc, DeltaTime, InterpSpeed);
        const FVector FromCharacter = InterpLoc - CharacterLocation;
        WeaponTransform.SetRotation(Direction.ToOrientationQuat());
        WeaponTransform.SetLocation(CharacterLocation + FromCharacter.GetSafeNormal() * WeaponDis);
    }
    else
    {
        // 保持和角色的相对位置不变
        FVector CharMove = GetActorLocation() - LastCharacterLocation;
        WeaponTransform.AddToTranslation(CharMove);
    }
    Server_UpdateWeaponTransform(WeaponTransform);
    //UE_LOG(LogProjectV, Display, TEXT("Weapon Location: %s"), *WeaponTransform.GetLocation().ToString());
}

void AV_PlayerCharacter::Server_ThrowupCurrentWeapon_Implementation()
{
    AV_WeaponBase* Weapon = GetCurrentWeapon();
    if (!IsValid(Weapon))
    {
        UE_LOG(LogProjectV, Log, TEXT("Server_ThrowupCurrentWeapon: No CurrentWeapon in %s"), *GetName());
        return;
    }

    UV_WeaponASC* WeaponASC = Cast<UV_WeaponASC>(Weapon->GetAbilitySystemComponent());
    if (IsValid(WeaponASC))
    {
        FGameplayTagContainer CancelTag;
        CancelTag.AddTag(VTags::VAbilites::VWeapons::WeaponReload);
        WeaponASC->CancelAbilities(&CancelTag);
    }

    if (CurrentWeaponIndex == static_cast<int8>(0))
    {
        check(IsValid(Weapon0));
        Weapon0->Thrownup();
        Weapon0 = nullptr;
        if (PlayerHUD.IsValid())
            PlayerHUD->SetWeapon0Icon(nullptr);
    }
    else if (CurrentWeaponIndex == static_cast<int8>(1))
    {
        check(IsValid(Weapon1));
        Weapon1->Thrownup();
        Weapon1 = nullptr;
        if (PlayerHUD.IsValid())
            PlayerHUD->SetWeapon1Icon(nullptr);
    }

    CurrentWeaponIndex = -1;
    if (ActiveEquipEffectHandle.IsValid())
    {
        PlayerASC->RemoveActiveGameplayEffect(ActiveEquipEffectHandle);
        ActiveEquipEffectHandle.Invalidate();
    }

    if (IsLocallyControlled())
    {
        BindWeaponUI();
    }
}

AV_WeaponBase* AV_PlayerCharacter::GetCurrentWeapon()
{
    switch (CurrentWeaponIndex)
    {
        case static_cast<int8>(0):
            return Weapon0;
        case static_cast<int8>(1):
            return Weapon1;
        default:
            return nullptr;
    }
}

void AV_PlayerCharacter::Server_EquipWeapon_Implementation(int8 WeaponIndex)
{
    if (WeaponIndex == CurrentWeaponIndex)
        return;

    // equip new weapon
    if (WeaponIndex == static_cast<int8>(0))
    {
        if (!IsValid(Weapon0))
        {
            UE_LOG(LogProjectV, Log, TEXT("Server_EquipWeapon: Weapon0 is null in %s"), *GetName());
            return;
        }
        Weapon0->Equipped();
    }
    else if (WeaponIndex == static_cast<int8>(1))
    {
        if (!IsValid(Weapon1))
        {
            UE_LOG(LogProjectV, Log, TEXT("Server_EquipWeapon: Weapon1 is null in %s"), *GetName());
            return;
        }
        Weapon1->Equipped();
    }

    // apply equip weapon gameplay effect
    check(PlayerASC.IsValid());
    // add effect when no active equip effect, prevent stacking
    if (!PlayerASC->GetActiveGameplayEffect(ActiveEquipEffectHandle))
    {
        FGameplayEffectContextHandle Context = PlayerASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = PlayerASC->MakeOutgoingSpec(EquipWeaponEffectClass, 1.0f, Context);
        check(SpecHandle.IsValid());
        ActiveEquipEffectHandle = PlayerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        UE_LOG(LogProjectV, Log, TEXT("Applied EquipWeaponEffect in %s, EquipEffectHandle:%s"), *GetName(),
               *ActiveEquipEffectHandle.ToString());
    }

    // unequip current weapon
    if (CurrentWeaponIndex == static_cast<int8>(0))
    {
        if (IsValid(Weapon0))
            Weapon0->Unequipped();
    }
    else if (CurrentWeaponIndex == static_cast<int8>(1))
    {
        if (IsValid(Weapon1))
            Weapon1->Unequipped();
    }

    // successfully equipped, update CurrentWeaponIndex
    CurrentWeaponIndex = WeaponIndex;

    if (IsLocallyControlled())
    {
        BindWeaponUI();
    }
}