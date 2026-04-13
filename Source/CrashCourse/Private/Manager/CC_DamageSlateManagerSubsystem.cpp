// Copyright WJW


#include "Manager/CC_DamageSlateManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "UI/Slate/CC_DamageTextCanvas.h"

void UCC_DamageSlateManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Warning, TEXT("%s Initialize"), *GetClass()->GetName());
}

bool UCC_DamageSlateManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	// 获取当前World
	const UWorld* World = Cast<UWorld>(Outer);
	// 专用服务器不创建，无需飘字
	return World != nullptr && !World->IsNetMode(NM_DedicatedServer);
}

void UCC_DamageSlateManagerSubsystem::Deinitialize()
{
	// 从视口中移除 Widget。
	if (GEngine && GEngine->GameViewport && DamageTextCanvas.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(DamageTextCanvas.ToSharedRef());
	}
	Super::Deinitialize();
}

void UCC_DamageSlateManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Subsystem 的 Initialize 时机无法保证 GameViewport 已经初始化。
	// 因此采用在 Tick 中添加 ViewportWidget 的方式。
	if (!bWidgetInitialized && GEngine && GEngine->GameViewport)
	{
		// 生成 Slate Widget。
		DamageTextCanvas = SNew(SCC_DamageTextCanvas, this);
		
		// 将生成的 Widget 添加到视口。
		GEngine->GameViewport->AddViewportWidgetContent(DamageTextCanvas.ToSharedRef(), 0);

		bWidgetInitialized = true;
	}

	// 有玩家角色时，才显示 Damage Text
	const APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController)
	{
		ActiveDamageNumbers.Empty();
		return;
	}
		
	// 遍历当前可能显示在屏幕上的 Damage Text，更新其值。
	// 倒序遍历，避免删除元素时导致索引错误。
	for (int32 i = ActiveDamageNumbers.Num() - 1; i >= 0; --i)
	{
		FDamageInfo& Info = ActiveDamageNumbers[i];

		Info.LifeTime -= DeltaTime;

		if (Info.LifeTime <= 0.f)
		{
			ActiveDamageNumbers.RemoveAt(i);
		}
		else
		{
			// 伤害数字上飘
			Info.VerticalOffset += DeltaTime * 30.f;
			// 获取屏幕坐标
			Info.bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, Info.HitLocation, Info.ScreenPosition, true);
		}
	}
}

TStatId UCC_DamageSlateManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCC_DamageSlateManagerSubsystem, STATGROUP_Game);
}

void UCC_DamageSlateManagerSubsystem::ShowDamageNumber(const float InDamageAmount, const FVector& InHitLocation)
{	
	FDamageInfo NewInfo;
	if (InDamageAmount <= 0.f)
	{
		NewInfo.DamageAmountText = TEXT("No Damage!");
	}
	else
	{
		const float RoundDamage = FMath::RoundToFloat(InDamageAmount);
		NewInfo.DamageAmountText = FString::SanitizeFloat(RoundDamage, 0);
	}
	NewInfo.HitLocation = InHitLocation;

	ActiveDamageNumbers.Add(NewInfo);
}

TArray<FDamageInfo>* UCC_DamageSlateManagerSubsystem::GetActiveDamageNumbers()
{
	return &ActiveDamageNumbers;
}
