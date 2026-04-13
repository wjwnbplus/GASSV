// Copyright WJW


#include "AbilitySystem/Abilities/Player/CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTags/CCTags.h"
#include "Utils/CC_BlueprintLibrary.h"

void UCC_Primary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
	// 冷却、耗能等判断
	if (!CommitAbility(Handle, ActorInfo,ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 等待Primary事件（在蒙太奇中调用）
	WaitPrimaryEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CCTags::Events::Player::Primary, nullptr, false, true);
	if (WaitPrimaryEvent)
	{
		WaitPrimaryEvent->EventReceived.AddDynamic(this, &UCC_Primary::OnPrimaryEventReceived);
		WaitPrimaryEvent->ReadyForActivation();			// 激活任务	
	}
	// 播放蒙太奇
	PlayAttackMontage();
}

void UCC_Primary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)
{
	for (AActor* HitActor : ActorsHit)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, CCTags::Events::Enemy::HitReact, Payload);
	}
}

void UCC_Primary::PlayAttackMontage()
{
	UAnimMontage* SelectedMontage = bFlipFlop ? MontageB : MontageA;
	bFlipFlop = !bFlipFlop;

	if (!SelectedMontage) return;

	// 播放蒙太奇
	if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SelectedMontage, 1.0f, NAME_None, true, 1.0f, 0.0f, false))
	{
		MontageTask->OnCompleted.AddDynamic(this, &UCC_Primary::OnMontageEnded);
		MontageTask->OnBlendOut.AddDynamic(this, &UCC_Primary::OnMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UCC_Primary::OnMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &UCC_Primary::OnMontageEnded);
		MontageTask->ReadyForActivation();
	}
}

void UCC_Primary::OnMontageEnded()
{
	if (WaitPrimaryEvent)
	{
		WaitPrimaryEvent->EndTask();
		WaitPrimaryEvent = nullptr;
	}
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UCC_Primary::PerformHitCheck()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	// 调用蓝图库：球形碰撞检测（和蓝图完全一致）
	TArray<AActor*> HitActors = UCC_BlueprintLibrary::HitBoxOverlapTest(
		Avatar,
		HitBoxRadius,
		HitBoxForwardOffset,
		HitBoxElevationOffset,
		bDrawDebugs
	);

	if (HitActors.Num() <= 0) return;

	// 发送受击反应
	SendHitReactEventToActors(HitActors);

	// 对所有命中目标施加伤害
	ApplyDamageToActors(HitActors);
}

void UCC_Primary::ApplyDamageToActors(const TArray<AActor*>& HitActors)
{
	
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
		
	if (!OwnerASC || !DamageEffect) return;

	float AbilityLevel = GetAbilityLevel();

	// 获取施法者的 EffectContext
	FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();

	AActor* Avatar = GetAvatarActorFromActorInfo();
	EffectContext.AddInstigator(Avatar, Avatar);
	
	// 遍历所有命中目标
	for (AActor* TargetActor : HitActors)
	{
		if (!TargetActor) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!TargetASC) continue;

		// 创建GameplayEffectSpec
		FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(DamageEffect, AbilityLevel, EffectContext);
		if (!SpecHandle.IsValid()) continue;

		// 应用伤害
		OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
	}
}

void UCC_Primary::OnPrimaryEventReceived(FGameplayEventData EventData)
{
	PerformHitCheck();
}
