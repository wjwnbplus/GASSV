// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "CC_BaseCharacter.h"
#include "CC_PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class CRASHCOURSE_API ACC_PlayerCharacter : public ACC_BaseCharacter
{
	GENERATED_BODY()

public:
	ACC_PlayerCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	// 控制器接管角色时调用（具体实现限定只在服务端执行）
	virtual void PossessedBy(AController* NewController) override;
	// PlayerState更新到客户端时调用，只在客户端执行，确保PlayerState已经存在
	virtual void OnRep_PlayerState() override;
	virtual void UnPossessed() override;

	virtual void RegisterPawn(APawn* InPawn) override;
	virtual void UnregisterPawn(APawn* InPawn) override;
	
	UFUNCTION(Client, Reliable)
	void Client_ShowDamage(const float InDamageAmount, const FVector& InHitLocation);

private:

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

};
