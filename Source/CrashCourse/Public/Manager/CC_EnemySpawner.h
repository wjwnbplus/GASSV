// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_EnemySpawner.generated.h"

class UEnvQuery;
struct FEnvQueryResult;

USTRUCT(BlueprintType)
struct FMonsterData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACharacter> MonsterCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	float MovementSpeed = 300.f;
};

UCLASS(BlueprintType)
class CRASHCOURSE_API UMonsterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<ACharacter> GetMonsterCharacterClass(const uint8 MonsterID);
	float GetMonsterSpeed(const uint8 MonsterID);

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<uint8, FMonsterData> MonsterData;
};


UCLASS()
class CRASHCOURSE_API ACC_EnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	ACC_EnemySpawner();
	
	UMonsterDataAsset* GetMonsterDataAsset();
	TSubclassOf<ACharacter> GetMonsterCharacterClass(const uint8 MonsterID) const;
	float GetMonsterSpeed(const uint8 MonsterID) const;
	
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

	void OnQueryFinished(TSharedPtr<FEnvQueryResult> EQSResult) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Monster")
	TObjectPtr<UMonsterDataAsset> MonsterDataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Monster")
	TObjectPtr<UEnvQuery> EnemySpawnLocationQuery;

};
