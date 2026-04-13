// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CC_PawnManagerSubsystem.generated.h"

/**
 *	角色管理
 */
UCLASS()
class CRASHCOURSE_API UCC_PawnManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()	

public:
	void RegisterPlayerPawn(APawn* InPawn);
	void UnregisterPlayerPawn(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	TArray<APawn*> GetAllPlayerPawns();
	
	void RegisterAIPawn(APawn* InPawn);
	void UnregisterAIPawn(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	TArray<APawn*> GetAllAIPawns();
	
	APawn* GetRandomPlayerPawn();
	APawn* GetLocalPlayerPawn();

	FVector GetNearestPlayerPawnNavLocation(const FVector& InLocation);

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<APawn>> PlayerPawns;

	UPROPERTY()
	TArray<TWeakObjectPtr<APawn>> AIPawns;

	UPROPERTY()
	TArray<TObjectPtr<APawn>> CachedPlayerPawns;
	
	UPROPERTY()
	TArray<TObjectPtr<APawn>> CachedAIPawns;

	bool bPlayerPawnsCacheDirty = true;
	bool bAIPawnsCacheDirty = true;
};

