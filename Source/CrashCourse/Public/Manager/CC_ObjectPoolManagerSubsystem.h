// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CC_ObjectPoolManagerSubsystem.generated.h"

/**
 *  TMap Value不能为TArray，用结构体包装
 */
USTRUCT()
struct FActorPoolContainer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> InactiveActors;
};

/**
 * 对象池优化角色创建销毁
 */
UCLASS()
class CRASHCOURSE_API UCC_ObjectPoolManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	inline static const FVector PoolLocation = FVector(0.f,0.f,-10000.f);

	// 模板声明
	template<typename T>
	T* GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn);
	// 非模板版本函数
	AActor* GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn);
	
	void ReturnToPool(AActor* InActor);

private:
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FActorPoolContainer> ActorPoolMap;
};

// 模板实现
template <typename T>
T* UCC_ObjectPoolManagerSubsystem::GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn)
{
	// 调用非模板版本函数
	return Cast<T>(GetFromPool(InClass, bIsSpawning, OwnerActor, InstigatorPawn));
}