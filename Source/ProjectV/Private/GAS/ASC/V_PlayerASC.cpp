#include "GAS/ASC/V_PlayerASC.h"
#include "VGameplayTags.h"
#include "ProjectV/ProjectV.h"

UV_PlayerASC::UV_PlayerASC()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UV_PlayerASC::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
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