#pragma once

#include "EditorUtilityWidget.h"
#include "TexturePackerUtilityWidget.generated.h"

UENUM()
enum class ETextureChannelSource: uint8
{
	TEXTURE_RED   UMETA(DisplayName = "Red Channel"),
	TEXTURE_GREEN UMETA(DisplayName = "Green Channel"),
	TEXTURE_BLUE  UMETA(DisplayName = "Blue Channel"),
	TEXTURE_ALPHA UMETA(DisplayName = "Alpha Channel"),
	INLINE        UMETA(DisplayName = "Inline"),
};

USTRUCT(BlueprintType)
struct FTexturePackerUtilityChannelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Channel")
	ETextureChannelSource Source = ETextureChannelSource::TEXTURE_RED;

	UPROPERTY(EditAnywhere, Category = "Channel",
		meta = (EditCondition = "Source==ETextureChannelSource::TEXTURE_RED || Source==ETextureChannelSource::TEXTURE_BLUE || Source==ETextureChannelSource::TEXTURE_GREEN || Source==ETextureChannelSource::TEXTURE_ALPHA",
			EditConditionHides))
	TObjectPtr<UTexture2D> SourceTexture;

	UPROPERTY(EditAnywhere, Category = "Channel",
		meta = (EditCondition = "Source==ETextureChannelSource::INLINE",
			EditConditionHides))
	float SourceInline = 0.0;

	UPROPERTY(EditAnywhere, Category = "Channel")
	bool bSwapUV = false;

	UPROPERTY()
	FName SwitchKey;

	UPROPERTY()
	FName TextureKey;

	UPROPERTY()
	FName InlineKey;

	UPROPERTY()
	FName SourceKey;

	UPROPERTY()
	FName UVSwapKey;
};

UCLASS()
class UTexturePackerUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

private:

	// -------------------------- Red Channel -----------------------------------
	UPROPERTY(EditAnywhere, Category="Channels")
	FTexturePackerUtilityChannelData  Red;

	UPROPERTY(EditAnywhere, Category = "Channels")
	FTexturePackerUtilityChannelData  Green;

	UPROPERTY(EditAnywhere, Category = "Channels")
	FTexturePackerUtilityChannelData  Blue;

	UPROPERTY(EditAnywhere, Category = "Channels",
		meta=(EditCondition="bUsealpha", EditConditionHides))
	FTexturePackerUtilityChannelData  Alpha;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="OutputOptions",
		meta=(DisplayName="sRGB", AllowPrivateAccess))
	bool bSRGB = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutputOptions",
		meta = (AllowPrivateAccess))
	bool bUseAlpha = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutputOptions",
		meta = (AllowPrivateAccess))
	FString AssetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutputOptions",
		meta = (AllowPrivateAccess))
	int OutputHeight = 256;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutputOptions",
		meta = (AllowPrivateAccess))
	int OutputWidth = 256;

public:

	UTexturePackerUtilityWidget();

protected:

	UFUNCTION(BlueprintNativeEvent, Category="Material")
	UMaterialInstanceDynamic* GetDisplayMaterial();
	virtual UMaterialInstanceDynamic* GetDisplayMaterial_Implementation() { return nullptr; }

	UFUNCTION(BlueprintCallable, Category="Material")
	void UpdatePreviewDisplay();

private:

	void UpdateChannelValue(
		UMaterialInstanceDynamic* Material,
		const FTexturePackerUtilityChannelData& Data);
};