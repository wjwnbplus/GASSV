// Copyright WJW


#include "Manager/CC_EnemySpawner.h"

#include "Algo/RandomShuffle.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Manager/CC_EnemyManagerSubsystem.h"


TSubclassOf<ACharacter> UMonsterDataAsset::GetMonsterCharacterClass(const uint8 MonsterID)
{
	if (const FMonsterData* Data = MonsterData.Find(MonsterID))
	{
		return Data->MonsterCharacterClass;
	}
	return nullptr;
}

float UMonsterDataAsset::GetMonsterSpeed(const uint8 MonsterID)
{
	if (const FMonsterData* Data = MonsterData.Find(MonsterID))
	{
		return Data->MovementSpeed;
	}
	return 0.f;
}

ACC_EnemySpawner::ACC_EnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

UMonsterDataAsset* ACC_EnemySpawner::GetMonsterDataAsset()
{
	return MonsterDataAsset;
}

TSubclassOf<ACharacter> ACC_EnemySpawner::GetMonsterCharacterClass(const uint8 MonsterID) const
{
	return MonsterDataAsset->GetMonsterCharacterClass(MonsterID);
}

float ACC_EnemySpawner::GetMonsterSpeed(const uint8 MonsterID) const
{
	return MonsterDataAsset->GetMonsterSpeed(MonsterID);
}

void ACC_EnemySpawner::SpawnEnemy()
{	
	// 用 PlayerController 获取
	TArray<APawn*> AllPlayerPawns;    
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				AllPlayerPawns.Add(Pawn);
			}
		}
	}
    // 如果没有玩家，直接返回
	if (AllPlayerPawns.IsEmpty())
	{
		return;
	}    
		
	for (APawn* PlayerPawn : AllPlayerPawns)
	{
		FEnvQueryRequest QueryRequest(EnemySpawnLocationQuery, PlayerPawn);
		QueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &ThisClass::OnQueryFinished);
	}
}

void ACC_EnemySpawner::OnQueryFinished(TSharedPtr<FEnvQueryResult> EQSResult) const
{
	if (!EQSResult) return;
	
	// 从EQS结果提取位置信息
	TArray<FVector> Locations;
	EQSResult->GetAllAsLocations(Locations);

	UE_LOG(LogTemp, Warning, TEXT("EQS get %d Locations"), Locations.Num());
	// 随机使用若干位置
	// TODO: 实现算法，综合考虑波次、时间、距离等因素
	Algo::RandomShuffle(Locations);
	Locations.SetNum(1);

	UE_LOG(LogTemp, Warning, TEXT("Random Choose Location %s"), *Locations[0].ToString());
	
	APawn* QueriedPawn = nullptr;
	if (EQSResult->Owner.IsValid())
	{
		QueriedPawn = Cast<APawn>(EQSResult->Owner);
	}

	if (UCC_EnemyManagerSubsystem* EnemyManagerSubsystem = GetWorld()->GetSubsystem<UCC_EnemyManagerSubsystem>())
	{
		EnemyManagerSubsystem->SpawnEnemy(1, Locations, QueriedPawn);
	}

}

