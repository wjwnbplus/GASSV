// Copyright WJW


#include "UI/Slate/CC_DamageTextCanvas.h"

#include "SlateOptMacros.h"
#include "Engine/Font.h"
#include "Fonts/FontMeasure.h"
#include "Manager/CC_DamageSlateManagerSubsystem.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SCC_DamageTextCanvas::Construct(const FArguments& InArgs, UCC_DamageSlateManagerSubsystem* InManager)
{
	Manager = InManager;
	SetVisibility(EVisibility::HitTestInvisible);
	const FString FontPath = TEXT("/Game/Assets/Fonts/David_Libre/DavidLibre-Medium_Font.DavidLibre-Medium_Font");
	Font = LoadObject<UFont>(nullptr, *FontPath);
}

int32 SCC_DamageTextCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 ResultLayerID = LayerId;

	const float DPI_Scale = AllottedGeometry.GetAccumulatedLayoutTransform().GetScale();
	if (!Manager.IsValid() || FMath::IsNearlyZero(DPI_Scale))
	{
		return ResultLayerID;
	}
	
	FSlateFontInfo FontInfo;
	if (Font)
	{
		FontInfo = FSlateFontInfo(Font, 24.f);
	}
	else
	{
		FontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
		FontInfo.Size = 24.f;
	}

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	for (const FDamageInfo& Info : *Manager->GetActiveDamageNumbers())
	{
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
