// Copyright WJW


#include "Manager/CC_ObjectPoolManagerSubsystem.h"

#include "Character/CC_EnemyCharacter.h"

AActor* UCC_ObjectPoolManagerSubsystem::GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning,
                                                    AActor* OwnerActor, APawn* InstigatorPawn)
{
	TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InClass).InactiveActors;
	
	// 对象池成员不够，创建角色返回
	if (ActorPool.IsEmpty())
	{
		bIsSpawning = true;

		const FTransform SpawnTransform = FTransform(PoolLocation);
		AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(
			InClass,
			SpawnTransform,
			OwnerActor,
			InstigatorPawn,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		SpawnedActor->FinishSpawning(SpawnTransform);
		return SpawnedActor;
	}

	// 对象池存在对象，直接返回
	bIsSpawning = false;
	AActor* PopActor = ActorPool.Pop();

	// 恢复对象状态
	PopActor->SetActorHiddenInGame(false);		//显示
	PopActor->SetActorEnableCollision(true);	//启用碰撞
	return PopActor;
}

void UCC_ObjectPoolManagerSubsystem::ReturnToPool(AActor* InActor)
{
	if (InActor)
	{
		TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InActor->GetClass()).InactiveActors;

		// 数量限定
		if (InActor->IsA(ACC_EnemyCharacter::StaticClass()) && ActorPool.Num() > 50)
		{
			InActor->Destroy();
			return;	// 超过数量，直接销毁，不回收
		}
		
		InActor->SetActorTickEnabled(false);		// 关闭Tick
		InActor->SetActorHiddenInGame(true);		// 隐藏
		InActor->SetActorEnableCollision(false);	// 禁用碰撞
		InActor->SetActorLocation(PoolLocation);	// 移到地下
		
		ActorPool.Emplace(InActor);					// 加入对象池
	}
}
