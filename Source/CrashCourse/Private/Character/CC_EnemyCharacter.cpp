// Copyright WJW


#include "Character/CC_EnemyCharacter.h"

#include "AIController.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Manager/CC_EnemyManagerSubsystem.h"
#include "Manager/CC_ObjectPoolManagerSubsystem.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;	
	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>("AttributeSet");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACC_EnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsBeingLaunched);
}

UAbilitySystemComponent* ACC_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ACC_EnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ACC_EnemyCharacter::StopMovementUntilLanded()
{
	bIsBeingLaunched = true;
	AAIController*AIController = Cast<AAIController>(GetController());
	if (!IsValid(AIController)) return;
	AIController->StopMovement();
	if (!LandedDelegate.IsAlreadyBound(this, &ACC_EnemyCharacter::EnableMovementOnLanded))
	{
		LandedDelegate.AddDynamic(this, &ACC_EnemyCharacter::EnableMovementOnLanded);
	}
}

void ACC_EnemyCharacter::OnSpawnFromPool()
{
	ResetAttributes();	
}

void ACC_EnemyCharacter::EnableMovementOnLanded(const FHitResult& Hit)
{
	bIsBeingLaunched = false;
	LandedDelegate.RemoveAll(this);
}

void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this,this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(),GetAttributeSet());
	// 敌人由AIController控制，只在服务端执行
	if (!HasAuthority()) return;
	GiveStartupAbilities();
	InitializeAttributes();
	
	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(CC_AttributeSet)) return;	
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void ACC_EnemyCharacter::HandleDeath()
{
	Super::HandleDeath();
	if (UWorld* World = GetWorld())
	{
		if (UCC_EnemyManagerSubsystem* EnemyManager = World->GetSubsystem<UCC_EnemyManagerSubsystem>())
		{
			EnemyManager->ReturnToEnemyPool(this);
		}
	}
}





