// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_Primary.generated.h"

class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class CRASHCOURSE_API UCC_Primary : public UCC_GameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SendHitReactEventToActors(const TArray<AActor*>& ActorsHit);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crash|Abilities")
	float HitBoxRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crash|Abilities")
	float HitBoxForwardOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crash|Abilities")
	float HitBoxElevationOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Animation")
	UAnimMontage* MontageA = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|Animation")
	UAnimMontage* MontageB = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crash|GameplayEffect")
	TSubclassOf<UGameplayEffect> DamageEffect = nullptr;

	bool bFlipFlop = true;

private:
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitPrimaryEvent = nullptr;

	UFUNCTION()
	void OnMontageEnded();
	UFUNCTION()
	void OnPrimaryEventReceived(FGameplayEventData EventData);
	
	void PlayAttackMontage();
	void PerformHitCheck();
	void ApplyDamageToActors(const TArray<AActor*>& HitActors);



};
