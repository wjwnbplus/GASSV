// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CC_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRASHCOURSE_API UCC_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	//
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	// 批量复制同步，客户端调用，解决网络延迟导致的时序混乱
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);

private:
	// 检查是否有自动激活的技能
	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);
};
