// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CC_EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCC_EnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class CRASHCOURSE_API ICC_EnemyInterface
{
	GENERATED_BODY()
public:
	virtual void OnSpawnFromPool() = 0;
};
