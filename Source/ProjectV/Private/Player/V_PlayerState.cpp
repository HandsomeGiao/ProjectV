// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/V_PlayerState.h"
#include "GAS/ASC/V_PlayerASC.h"
#include "GAS/AttributeSet/V_PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "ProjectV/ProjectV.h"

AV_PlayerState::AV_PlayerState()
{
    ASC = CreateDefaultSubobject<UV_PlayerASC>(TEXT("AbilitySystemComponent"));
    ASC->SetIsReplicated(true);
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UV_PlayerAttributeSet>(TEXT("AttributeSet"));

    // 太慢的网络同步速率会导致属性变化太慢，影响体验
    SetNetUpdateFrequency(30.f);
}

void AV_PlayerState::SetVPlayerName(const FString& NewName)
{
    if (!HasAuthority())
    {
        UE_LOG(LogProjectV, Warning, TEXT("SetVPlayerName called on non-authority instance in %s"), *GetName());
        return;
    }
    VPlayerName = NewName;

    if (GetNetMode()!= NM_DedicatedServer)
    {
        OnVPlayerNameChanged.Broadcast(VPlayerName);
    }
}

void AV_PlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AV_PlayerState, VPlayerName);
}

void AV_PlayerState::OnRep_VPlayerName()
{
    OnVPlayerNameChanged.Broadcast(VPlayerName);
}