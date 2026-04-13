// Copyright WJW


#include "CrashCourse/Public/Character/CC_PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Manager/CC_DamageSlateManagerSubsystem.h"
#include "Manager/CC_PawnManagerSubsystem.h"
#include "Player/CC_PlayerState.h"


ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Tags.Add(CrashTags::Player);
}

UAbilitySystemComponent* ACC_PlayerCharacter::GetAbilitySystemComponent() const
{
	ACC_PlayerState* CCPlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState)) return nullptr;

	return CCPlayerState->GetAbilitySystemComponent();
}

UAttributeSet* ACC_PlayerCharacter::GetAttributeSet() const
{
	ACC_PlayerState* CCPlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState)) return nullptr;

	return CCPlayerState->GetAttributeSet();
}

void ACC_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// 只在服务器执行
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;

	RegisterPawn(this);
	UE_LOG(LogTemp, Warning, TEXT("[PlayerCharacter] Registered player pawn: %s"), *GetName());
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(),GetAttributeSet());

	GiveStartupAbilities(); // 服务端授予能力（会自动复制到客户端）
	InitializeAttributes();

	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(CC_AttributeSet)) return;
	
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

}

void ACC_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(),GetAttributeSet());

	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(CC_AttributeSet)) return;
	
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
}

void ACC_PlayerCharacter::UnPossessed()
{
	UnregisterPawn(this);
	Super::UnPossessed();
}

void ACC_PlayerCharacter::RegisterPawn(APawn* InPawn)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UCC_PawnManagerSubsystem>()->RegisterPlayerPawn(this);
	}
}

void ACC_PlayerCharacter::UnregisterPawn(APawn* InPawn)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UCC_PawnManagerSubsystem>()->UnregisterPlayerPawn(this);
	}
}

void ACC_PlayerCharacter::Client_ShowDamage_Implementation(const float InDamageAmount, const FVector& InHitLocation)
{
	UWorld* World = GetWorld();

	/**
	 * 对 Client调用逻辑的探索：敌人类角色仍然会在服务端调用
	const ENetMode NetMode = World->GetNetMode();
	const FString NetModeStr = (NetMode == NM_Standalone) ? TEXT("Standalone") :
							   (NetMode == NM_DedicatedServer) ? TEXT("DedicatedServer") :
							   (NetMode == NM_ListenServer) ? TEXT("ListenServer") :
							   (NetMode == NM_Client) ? TEXT("Client") : TEXT("Unknown");
	const ENetRole NetRole = GetLocalRole();
	const FString RoleStr = (NetRole == ROLE_Authority) ? TEXT("Authority") :
						   (NetRole == ROLE_AutonomousProxy) ? TEXT("AutonomousProxy") :
						   (NetRole == ROLE_SimulatedProxy) ? TEXT("SimulatedProxy") : TEXT("Unknown");
	const FString HasAuthorityStr = HasAuthority() ? TEXT("HasAuthority") : TEXT("NoAuthority");
	
	UE_LOG(LogTemp, Warning, TEXT("Client ShowDamage"));
	UE_LOG(LogTemp, Warning, TEXT("Subsystem: %s"), *GetClass()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("NetMode: %d (%s)"), static_cast<int32>(NetMode), *NetModeStr);
	UE_LOG(LogTemp, Warning, TEXT("NetRole: %d (%s)"), static_cast<int32>(NetRole), *RoleStr);
	UE_LOG(LogTemp, Warning, TEXT("HasAuthority:%s"), *HasAuthorityStr);
	*/
	
	if (UCC_DamageSlateManagerSubsystem* DamageSubsystem = World->GetSubsystem<UCC_DamageSlateManagerSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem: %s"), *DamageSubsystem->GetName());
		DamageSubsystem->ShowDamageNumber(InDamageAmount, InHitLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client_ShowDamage: Subsystem not found"));
	}
}


