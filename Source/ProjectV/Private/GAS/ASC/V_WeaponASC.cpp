// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ASC/V_WeaponASC.h"

#include "VGameplayTags.h"
#include "ProjectV/ProjectV.h"

void UV_WeaponASC::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    Super::OnGiveAbility(AbilitySpec);

    UE_LOG(LogProjectV, Log, TEXT("Ability Given: %s in %s"), *AbilitySpec.Ability->GetName(),
           GetOwner()->HasAuthority()?TEXT("Server"):TEXT("Client"));

    for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
    {
        if (Tag.MatchesTag(VTags::VAbilites::ActivateOnGiven))
        {
            TryActivateAbility(AbilitySpec.Handle);
            break;
        }
    }
}