#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "V_BaseCharacter.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class PROJECTV_API AV_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AV_BaseCharacter();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return nullptr; }

protected:
    // ======= override func =======
    virtual void BeginPlay() override;

    // ======= 内部方法 =======
    void GiveStartupAbilities();
    void InitializeAttributes();
    
private:

    // ========= 配置变量 ========
    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

    UPROPERTY(EditDefaultsOnly, Category="GAS")
    TSubclassOf<UGameplayEffect> ResetAttributesEffect;
};