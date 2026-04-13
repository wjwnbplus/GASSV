// Copyright WJW

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CC_DamageFontConfig.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CRASHCOURSE_API UCC_DamageFontConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UFont* GetFont() const { return DamageNumberFont; }
	float GetFontSize() const { return FontSize; }
private:
	UPROPERTY(EditDefaultsOnly)
	UFont* DamageNumberFont;  // 在编辑器中指定
    
	UPROPERTY(EditDefaultsOnly)
	float FontSize = 24.f;
};
