// Copyright WJW


#include "AbilitySystem/ExecCalc/CC_ExecCalc_Damage.h"

#include "AbilitySystem/CC_AttributeSet.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCC_AttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCC_AttributeSet, Attack, Source, false);
	}

};

//单例获取函数
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}


UCC_ExecCalc_Damage::UCC_ExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}


void UCC_ExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 获取Spec，包含效果所有配置信息
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// 获取SourceTags和TargetTags，存入 EvaluationParameters
	// CapturedSourceTags是标签捕获器，负责在 GameplayEffect 应用时捕获来源方的标签。
	// GetAggregatedTags() 返回聚合后的标签集合，即所有伤害计算时需要的标签
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	// FAggregatorEvaluateParameters 是一个上下文传递结构体，用于在属性计算时传递必要的评估信息。
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	
	float SourceAttack = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude
	(
		DamageStatics().AttackDef,	// 属性捕获定义
		EvaluationParameters,		// 评估参数
		SourceAttack				// 输出结果
	);

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	
	float Damage = SourceAttack - TargetArmor;
	Damage = FMath::Clamp(Damage, 0.f, Damage);

	// 将计算完成的伤害值打包成修饰符数据，并添加到输出队列中
	const FGameplayModifierEvaluatedData EvaluatedData(UCC_AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData); //修饰符集合容器
}















