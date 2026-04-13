// Copyright WJW


#include "Character/CC_EnemyCharacter.h"

#include "AIController.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Manager/CC_EnemyManagerSubsystem.h"
#include "Manager/CC_PawnManagerSubsystem.h"
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


void ACC_EnemyCharacter::RegisterPawn(APawn* InPawn)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UCC_PawnManagerSubsystem>()->RegisterAIPawn(this);
	}
}

void ACC_EnemyCharacter::UnregisterPawn(APawn* InPawn)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UCC_PawnManagerSubsystem>()->UnregisterAIPawn(this);
	}
}

void ACC_EnemyCharacter::EnableMovementOnLanded(const FHitResult& Hit)
{
	bIsBeingLaunched = false;
	LandedDelegate.RemoveAll(this);
}



void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// GAS 初始化
	if (!IsValid(GetAbilitySystemComponent())) return;
	GetAbilitySystemComponent()->InitAbilityActorInfo(this,this);
	// 自定义广播，方便UI等处监听
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(),GetAttributeSet());

	// 首次创建，执行一次出池逻辑
	OnSpawnFromPool();
}

void ACC_EnemyCharacter::OnSpawnFromPool()
{
	if (!HasAuthority()) return;
	GiveStartupAbilities();
	InitializeAttributes();
	
	// 重新注册监听器（防止回收时被移除）
	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (IsValid(CC_AttributeSet))
	{
		// 先移除旧的，防止重复注册
		// 委托存在ASC中，出入池需要清理
		GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).RemoveAll(this);
		GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	}
	SetAlive(true);
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





