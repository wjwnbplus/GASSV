// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CC_AttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributesInitialized);

/**
 * 信息提取，方便获取属性
 */
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};
 
UCLASS()
class CRASHCOURSE_API UCC_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintAssignable)
	FAttributesInitialized OnAttributesInitialized;

	UPROPERTY(ReplicatedUsing = OnRep_AttributesInitialized)
	bool bAttributesInitialized = false;

	UFUNCTION()
	void OnRep_AttributesInitialized();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "CC|Attributes")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "CC|Attributes")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "CC|Attributes")
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "CC|Attributes")
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "CC|Attributes")
	FGameplayAttributeData Attack;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAttack, Category = "CC|Attributes")
	FGameplayAttributeData MaxAttack;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "CC|Attributes")
	FGameplayAttributeData Armor;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor, Category = "CC|Attributes")
	FGameplayAttributeData MaxArmor;
	
	/**
	 * 元属性（Meta Attribute）
	 * 
	 * 当数值发生变化时，我们不会直接将其应用于上方声明的属性，而是声明并使用一个充当中间过渡作用的属性。
	 * 元属性**不会被网络复制**。
	 * 
	 * 若不使用元属性而直接应用效果，各类效果的计算顺序会变得至关重要。
	 * 例如：若将伤害减免效果放在最后计算，即便角色当前生命值为正数，也可能出现生命值瞬间降至0以下的情况，
	 * 从而导致角色非预期死亡。
	 * 
	 * 使用元属性后，当多种效果（伤害减免、额外伤害等）同时生效时，无需过度关注逻辑执行顺序，
	 * 并且能够天然避免效果重复应用、遗漏计算等问题。
	 */	
	UPROPERTY(BlueprintReadOnly, Category = "CC|Attributes")
	FGameplayAttributeData IncomingDamage;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAttack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldValue);

	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);
	ATTRIBUTE_ACCESSORS(ThisClass, Attack);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxAttack);
	ATTRIBUTE_ACCESSORS(ThisClass, Armor);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxArmor);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingDamage);

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const;
	void ApplyIncomingDamage(const FEffectProperties& Props, const FGameplayEffectModCallbackData& Data);

};
