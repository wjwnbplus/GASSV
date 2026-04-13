// Copyright WJW


#include "UI/Slate/CC_DamageTextCanvas.h"

#include "SlateOptMacros.h"
#include "Fonts/FontMeasure.h"
#include "Engine/Font.h"
#include "Manager/CC_DamageSlateManagerSubsystem.h"
#include "UI/Slate/CC_DamageFontConfig.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SCC_DamageTextCanvas::Construct(const FArguments& InArgs, UCC_DamageSlateManagerSubsystem* InManager)
{
	UE_LOG(LogTemp, Warning, TEXT("State Construct"));
	Manager = InManager;
	SetVisibility(EVisibility::HitTestInvisible);
	if (Manager.IsValid())
	{
		FontConfig = Manager->GetFontConfig();
	}
}

int32 SCC_DamageTextCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 ResultLayerID = LayerId;

	// 获取DPI缩放，用于计算绘制位置。
	const float DPI_Scale = AllottedGeometry.GetAccumulatedLayoutTransform().GetScale();
	if (!Manager.IsValid() || FMath::IsNearlyZero(DPI_Scale))
	{
		UE_LOG(LogTemp, Error, TEXT("Early return because: ManagerValid=%d, DPI_Scale=%.6f"), Manager.IsValid(), DPI_Scale);
		return ResultLayerID;
	}
	
	FSlateFontInfo FontInfo;
	if (FontConfig.IsValid())
	{
		FontInfo = FSlateFontInfo(FontConfig->GetFont(),  FontConfig->GetFontSize());
	}
	else
	{
		FontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
		FontInfo.Size = 24.f;
	}

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	TArray<FDamageInfo>* ActiveDamages = Manager->GetActiveDamageNumbers();
	const int32 DamageCount = ActiveDamages->Num();
	
	for (int32 i = 0; i < DamageCount; ++i)
	{
		const FDamageInfo& Info = (*ActiveDamages)[i];
		
		// 只有当此次 Damage Text 的 3D 位置实际绘制在屏幕上时才进入此分支。
		if (Info.bIsOnScreen)
		{
			// 预先计算要绘制的文本大小。
			const FVector2D TextSize = FontMeasure->Measure(Info.DamageAmountText, FontInfo);

			// 从原始位置向左移动文本宽度的一半以实现居中，并根据分辨率重新计算。
			FVector2D DrawPosition = Info.ScreenPosition / DPI_Scale;
			DrawPosition.X -= TextSize.X * 0.5f;
			// 添加逐渐向上移动的动画效果。
			DrawPosition.Y -= Info.VerticalOffset;

			// 根据剩余寿命调整透明度。
			float Alpha = Info.LifeTime > 0.5f ? 1.f : Info.LifeTime / 0.5f;
			FLinearColor TextColor = FLinearColor(1.f, 0.8f, 0.2f, Alpha);
			
			FSlateDrawElement::MakeText(
				OutDrawElements,
				++ResultLayerID,
				AllottedGeometry.ToPaintGeometry(FVector2D(1,1), FSlateLayoutTransform(DrawPosition)),
				Info.DamageAmountText,
				FontInfo,
				ESlateDrawEffect::None,
				TextColor
				);
		}
	}

	return ResultLayerID;
}
