// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/V_PlayerHUD.h"

#include "AbilitySystemComponent.h"
#include "UI/V_PlayerMainLayout.h"
#include "Blueprint/UserWidget.h"
#include "GAS/ASC/V_PlayerASC.h"
#include "GAS/AttributeSet/V_PlayerAttributeSet.h"
#include "GAS/AttributeSet/V_WeaponAttributeSet.h"
#include "Player/V_PlayerCharacter.h"
#include "ProjectV/ProjectV.h"

void AV_PlayerHUD::InitLayout(AV_PlayerCharacter* InPlayerChar)
{
    // this func might be called multiple, so prevent second init
    if (PlayerChar.IsValid() && IsValid(PlayerMainLayout))
    {
        UE_LOG(LogProjectV, Log, TEXT("InitLayout called multiple times in %s"), *GetName());
        return;
    }

    PlayerMainLayout = CreateWidget<UV_PlayerMainLayout>(GetWorld(), PlayerMainLayoutClass);
    if (PlayerMainLayout)
    {
        PlayerMainLayout->AddToViewport();
    }

    if (IsValid(InPlayerChar))
    {
        PlayerChar = InPlayerChar;
        UV_PlayerASC* InASC = Cast<UV_PlayerASC>(InPlayerChar->GetAbilitySystemComponent());
        check(IsValid(InASC));
        // 注意，由于不知道当前装备了什么武器，因此无法在这里绑定MaxAmmo（player ammo）
        PlayerASCDelegateHandles[0] =
                InASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetHealthAttribute()).
                       AddLambda(
                               [this](const FOnAttributeChangeData& Data)
                               {
                                   SetHealth(Data.NewValue);
                               });
        PlayerASCDelegateHandles[1] =
                InASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetMaxHealthAttribute()).
                       AddLambda(
                               [this](const FOnAttributeChangeData& Data)
                               {
                                   SetMaxHealth(Data.NewValue);
                               });
        // UE_LOG(LogProjectV, Warning, TEXT("Health Value = %f, Max Health Value = %f"),
        //        InASC->GetNumericAttribute(UV_PlayerAttributeSet::GetHealthAttribute()),
        //        InASC->GetNumericAttribute(UV_PlayerAttributeSet::GetMaxHealthAttribute()));

        // init health values
        SetMaxHealth(InASC->GetNumericAttribute(UV_PlayerAttributeSet::GetMaxHealthAttribute()));
        SetHealth(InASC->GetNumericAttribute(UV_PlayerAttributeSet::GetHealthAttribute()));
    }
}

// 绑定当前装备武器的弹药，以及剩余弹药
void AV_PlayerHUD::SetWeapon(AV_WeaponBase* InWeapon)
{
    RemoveInvalidDelegate();
    if (IsValid(InWeapon))
    {
        WeaponASC = InWeapon->GetAbilitySystemComponent();
        WeaponASCDelegateHandles[0] =
                WeaponASC->GetGameplayAttributeValueChangeDelegate(UV_WeaponAttributeSet::GetCurrentAmmoAttribute()).
                           AddLambda(
                                   [this](const FOnAttributeChangeData& Data)
                                   {
                                       SetCurrentAmmo(Data.NewValue);
                                   });
        // bind weapon reserve ammo delegate here
        if (PlayerChar.IsValid())
        {
            ReserverAmmoChangeDelegate = &PlayerChar->GetAmmoChangeDelegate();
            PlayerASCDelegateHandles[2] = ReserverAmmoChangeDelegate->AddLambda(
                    [this](const FOnAttributeChangeData& Data)
                    {
                        SetReserveAmmo(Data.NewValue);
                    });
            // init when equip
            SetReserveAmmo(PlayerChar->GetReserveAmmo());
        }

        // init ammo values
        SetCurrentAmmo(WeaponASC->GetNumericAttribute(UV_WeaponAttributeSet::GetCurrentAmmoAttribute()));
    }
    else
    {
        // 传入nullptr，代表清空绑定
        SetCurrentAmmo(0);
        SetReserveAmmo(0);
    }
}

void AV_PlayerHUD::SetHealth(float NewHealth)
{
    if (IsValid(PlayerMainLayout))
        PlayerMainLayout->SetHealth(NewHealth);
}

void AV_PlayerHUD::SetMaxHealth(float NewMaxHealth)
{
    if (IsValid(PlayerMainLayout))
        PlayerMainLayout->SetMaxHealth(NewMaxHealth);
}

void AV_PlayerHUD::SetCurrentAmmo(float CurrentAmmo)
{
    if (IsValid(PlayerMainLayout))
        PlayerMainLayout->SetAmmo(CurrentAmmo);
}

void AV_PlayerHUD::SetReserveAmmo(float MaxAmmo)
{
    if (IsValid(PlayerMainLayout))
        PlayerMainLayout->SetMaxAmmo(MaxAmmo);
}

void AV_PlayerHUD::SetWeapon0Icon(UTexture2D* Icon)
{
    if (IsValid(PlayerMainLayout))
        PlayerMainLayout->SetWeapon0Icon(Icon);
}

void AV_PlayerHUD::SetWeapon1Icon(UTexture2D* Icon)
{
    if (IsValid(PlayerMainLayout))
        PlayerMainLayout->SetWeapon1Icon(Icon);
}

void AV_PlayerHUD::RemoveInvalidDelegate()
{
    if (WeaponASC.IsValid())
    {
        if (WeaponASCDelegateHandles[0].IsValid())
            WeaponASC->GetGameplayAttributeValueChangeDelegate(UV_WeaponAttributeSet::GetCurrentAmmoAttribute()).Remove(
                    WeaponASCDelegateHandles[0]);
        WeaponASCDelegateHandles[0].Reset();
    }

    // 首先移除之前绑定的PlayerASC的ReserveAmmo delegate
    if (PlayerChar.IsValid())
    {
        if (ReserverAmmoChangeDelegate && PlayerASCDelegateHandles[2].IsValid())
        {
            ReserverAmmoChangeDelegate->Remove(PlayerASCDelegateHandles[2]);
            PlayerASCDelegateHandles[2].Reset();
        }
    }
}

void AV_PlayerHUD::Destroyed()
{
    Super::Destroyed();

    // remove delegates
    if (PlayerChar.IsValid())
    {
        UAbilitySystemComponent* PlayerASC = PlayerChar->GetAbilitySystemComponent();
        if (IsValid(PlayerASC))
        {
            if (PlayerASCDelegateHandles[0].IsValid())
            {
                PlayerASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetHealthAttribute()).Remove(
                        PlayerASCDelegateHandles[0]);
                PlayerASCDelegateHandles[0].Reset();
            }
            if (PlayerASCDelegateHandles[1].IsValid())
            {
                PlayerASC->GetGameplayAttributeValueChangeDelegate(UV_PlayerAttributeSet::GetMaxHealthAttribute()).
                           Remove(
                                   PlayerASCDelegateHandles[1]);
                PlayerASCDelegateHandles[1].Reset();
            }
        }
    }

    RemoveInvalidDelegate();
}