// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CC_EnemyManagerSubsystem.generated.h"

class ACC_EnemyCharacter;
class ACC_EnemySpawner;
/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class CRASHCOURSE_API UCC_EnemyManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin FTickableGameObject Interface
	// 引擎调用
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UCC_EnemyManagerSubsystem, STATGROUP_Tickables);
	}
	//~ End of FTickableGameObject Interface

	// EntitySpawner 通过 EQS 生成随机位置后调用此函数。
	void SpawnEnemy(const uint8 MonsterID, const TArray<FVector>& NewLocations, APawn* TargetPawn);

	void ReturnToEnemyPool(ACC_EnemyCharacter* Enemy) const;
private:
	void InitEnemySpawner();

	ACharacter* GetEnemyFromPool(uint8 InMonsterID) const;


private:
	UPROPERTY(EditAnywhere, Category = "Crash")
	TSubclassOf<ACC_EnemySpawner> EnemySpawnerClass;

	UPROPERTY()
	TObjectPtr<ACC_EnemySpawner> GlobalEnemySpawner;
};
