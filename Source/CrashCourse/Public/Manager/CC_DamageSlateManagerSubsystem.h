// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CC_DamageSlateManagerSubsystem.generated.h"

class UCC_DamageFontConfig;
class UCC_DamageSlateConfig;
class SCC_DamageTextCanvas;

USTRUCT()
struct FDamageInfo
{
	GENERATED_BODY()

	FString DamageAmountText = TEXT("");
	
	FVector HitLocation = FVector::ZeroVector;

	float LifeTime = 2.f;

	float VerticalOffset = 0.f;

	FVector2D ScreenPosition = FVector2D::ZeroVector;

	bool bIsOnScreen = false;
};

/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class CRASHCOURSE_API UCC_DamageSlateManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin UWorldSubsystem Interface
	//生命周期函数，当游戏实例关闭、或者地图切换导致 Subsystem 被销毁时，引擎会自动调用这个函数。
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Deinitialize() override;
	//~ End of UWorldSubsystem Interface
	
	//~ Begin FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	// Tick 函数的执行时间，用于在 stat game 中显示的性能分析函数。
	virtual TStatId GetStatId() const override;
	//~ End of FTickableGameObject Interface

	void ShowDamageNumber(const float InDamageAmount, const FVector& InHitLocation);

	TArray<FDamageInfo>* GetActiveDamageNumbers();

	UCC_DamageFontConfig* GetFontConfig() const {return FontConfig;}

private:
		
	UPROPERTY()
	TArray<FDamageInfo> ActiveDamageNumbers;

	TSharedPtr<SCC_DamageTextCanvas> DamageTextCanvas;

	uint8 bWidgetInitialized : 1 = false;

	UPROPERTY(EditDefaultsOnly, Category = "Crash")
	TObjectPtr<UCC_DamageFontConfig> FontConfig;
};

