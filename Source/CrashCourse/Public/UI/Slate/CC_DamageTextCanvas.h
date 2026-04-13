// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UCC_DamageFontConfig;
class UCC_DamageSlateManagerSubsystem;


/**
 * 一次性绘制多个伤害数字，OnPaint自动调用
 */
class CRASHCOURSE_API SCC_DamageTextCanvas : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCC_DamageTextCanvas) {}
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UCC_DamageSlateManagerSubsystem* InManager);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	TWeakObjectPtr<UCC_DamageSlateManagerSubsystem> Manager;

	TWeakObjectPtr<UCC_DamageFontConfig> FontConfig;
};