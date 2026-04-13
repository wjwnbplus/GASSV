// Copyright WJW


#include "Manager/CC_EnemyManagerSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Character/CC_EnemyCharacter.h"
#include "GameplayTags/CCTags.h"
#include "Interface/CC_EnemyInterface.h"
#include "Manager/CC_EnemySpawner.h"
#include "Manager/CC_ObjectPoolManagerSubsystem.h"

void UCC_EnemyManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Warning, TEXT("EnemyManagerSubsystem Initialized"));
	
	if (GetWorld() && GetWorld()->IsNetMode(NM_Client))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyManager] Skipping initialization on Client."));
		return;
	}

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyManager] Initializing EnemyManagerSubsystem on Server"));
		InitEnemySpawner();
	}
}

void UCC_EnemyManagerSubsystem::SpawnEnemy(const uint8 MonsterID, const TArray<FVector>& NewLocations,
	APawn* TargetPawn)
{
	if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}
	
	if (NewLocations.IsEmpty())
	{
		return;
	}
	
	if (!GlobalEnemySpawner)
	{
		return;
	}

	// 遍历所有位置，生成 Character
	for (const FVector& Location : NewLocations)
	{
		// 从对象池获取敌人
		ACharacter* SpawnedEnemy = GetEnemyFromPool(MonsterID);
		if (!SpawnedEnemy)
		{
			continue;
		}
		UE_LOG(LogTemp, Warning, TEXT("Spawn Enemy at %s"), *Location.ToString());
		SpawnedEnemy->SetActorLocation(Location);
	}
}

void UCC_EnemyManagerSubsystem::InitEnemySpawner()
{	
	if (!GlobalEnemySpawner)
	{				
		if (!EnemySpawnerClass) return;
			
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		UWorld* World = GetWorld();
		if (!World) return;
		GlobalEnemySpawner = World->SpawnActor<ACC_EnemySpawner>(EnemySpawnerClass, SpawnParameters);		
	}
}

ACharacter* UCC_EnemyManagerSubsystem::GetEnemyFromPool(uint8 InMonsterID) const
{
	if (UCC_ObjectPoolManagerSubsystem* ObjectPoolManager = GetWorld()->GetGameInstance()->GetSubsystem<UCC_ObjectPoolManagerSubsystem>())
	{
		// 从 EntitySpawner 获取敌人 Class
		const TSubclassOf<ACharacter> EnemyClass = GlobalEnemySpawner->GetMonsterCharacterClass(InMonsterID);
		if (!EnemyClass)
		{
			return nullptr;
		}

		bool bIsSpawning;
		ACharacter* SpawnedEnemy = ObjectPoolManager->GetFromPool<ACharacter>(EnemyClass, bIsSpawning, nullptr, nullptr);

		if (SpawnedEnemy)
		{
			// 触发出池回调（重新应用属性等）
			if (ICC_EnemyInterface* EnemyInterface = Cast<ICC_EnemyInterface>(SpawnedEnemy))
			{
				EnemyInterface->OnSpawnFromPool();
			}
		}

		return SpawnedEnemy;
	}

	return nullptr;
}

void UCC_EnemyManagerSubsystem::ReturnToEnemyPool(ACC_EnemyCharacter* Enemy) const
{
	UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		// 1. 取消所有正在执行的能力
		ASC->CancelAllAbilities();
		
		// 2. 清除所有能力定义
		ASC->ClearAllAbilities();
		
		// 3. 移除所有激活的 Gameplay Effects（buff/debuff）
		TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = ASC->GetActiveGameplayEffects().GetAllActiveEffectHandles();
		for (const FActiveGameplayEffectHandle& Handle : ActiveEffectHandles)
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing GE: %s"), *Handle.ToString());
			ASC->RemoveActiveGameplayEffect(Handle);
		}

		ASC->RemoveLooseGameplayTag(CCTags::Status::Dead);
		
		UE_LOG(LogTemp, Warning, TEXT("Cleaned ASC for %s before returning to pool"), *Enemy->GetName());
	}
	if (UCC_ObjectPoolManagerSubsystem* ObjectPoolManager = GetWorld()->GetGameInstance()->GetSubsystem<UCC_ObjectPoolManagerSubsystem>())
	{
		ObjectPoolManager->ReturnToPool(Enemy);
	}
}
