// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CC_CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCC_CombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 角色注册，战斗交互
 */
class CRASHCOURSE_API ICC_CombatInterface
{
	GENERATED_BODY()

public:
	virtual void RegisterPawn(APawn* InPawn);
	virtual void UnregisterPawn(APawn* InPawn);
	
	//virtual void ApplyKnockback(const FVector& KnockbackForce) = 0;
	//virtual void Die() = 0;
};
