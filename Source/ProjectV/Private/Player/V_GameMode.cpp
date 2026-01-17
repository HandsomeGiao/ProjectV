// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/V_GameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Player/V_PlayerState.h"

void AV_GameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // Set Player Name
    if (AV_PlayerState* PS = Cast<AV_PlayerState>(NewPlayer->PlayerState))
    {
        PS->SetVPlayerName(UGameplayStatics::ParseOption(OptionsString,TEXT("Name")));
    }
}

void AV_GameMode::BeginPlay()
{
    Super::BeginPlay();
}

AV_GameMode::AV_GameMode()
{
    bUseSeamlessTravel = true;
}

void AV_GameMode::BeginSeamlessTravel(FString URL)
{
    GetWorld()->ServerTravel(URL, true);
}