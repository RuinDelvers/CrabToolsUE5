#include "EditorUtilityWidgets/TexturePackerUtilityWidget.h"
#include "Engine/TextureRenderTarget2D.h"

FTexturePackerInnerData::FTexturePackerInnerData()
{
	Red.SwitchKey = "UseRedTexture";
	Red.InlineKey = "RedChannelInline";
	Red.TextureKey = "RedChannel";
	Red.SourceKey = "RedChannelSource";
	Red.UVSwapKey = "RedSwapUV";

	Green.SwitchKey = "UseGreenTexture";
	Green.InlineKey = "GreenChannelInline";
	Green.TextureKey = "GreenChannel";
	Green.SourceKey = "GreenChannelSource";
	Green.UVSwapKey = "GreenSwapUV";

	Blue.SwitchKey = "UseBlueTexture";
	Blue.InlineKey = "BlueChannelInline";
	Blue.TextureKey = "BlueChannel";
	Blue.SourceKey = "BlueChannelSource";
	Blue.UVSwapKey = "BlueSwapUV";

	Alpha.SwitchKey = "UseAlphaTexture";
	Alpha.InlineKey = "AlphaChannelInline";
	Alpha.TextureKey = "AlphaChannel";
	Alpha.SourceKey = "AlphaChannelSource";
	Alpha.UVSwapKey = "AlphaSwapUV";
}


UTexturePackerUtilityWidget::UTexturePackerUtilityWidget()
{
	
}

void UTexturePackerUtilityWidget::UpdatePreviewDisplay()
{
	auto Preview = this->GetDisplayMaterial();
	check(Preview);

	UTexturePackerAssetData::ApplyPackedTextureDataToMaterial(Preview, this->Data);
}

void UTexturePackerUtilityWidget::ApplyToRenderTarget(UTextureRenderTarget2D* RenderTarget) const
{
	UTexturePackerAssetData::ApplyPackedTextureDataToRenderTarget(RenderTarget, this->Data);
}

void UTexturePackerUtilityWidget::ApplyExtraSettingsToTexture(UTexture2D* NewTexture) const
{
	check(NewTexture);

	NewTexture->CompressionNoAlpha = !this->Data.bUseAlpha;
	NewTexture->SRGB = this->Data.bSRGB;

	// Call this, or a crash will happen.
	NewTexture->PostEditChange();
}

void UTexturePackerAssetData::ApplyPackedTextureAssetDataToMaterial(UMaterialInstanceDynamic* Material) const
{
	ApplyPackedTextureDataToMaterial(Material, this->Data);
}

void UTexturePackerAssetData::ApplyPackedTextureAssetDataToRenderTarget(UTextureRenderTarget2D* RenderTarget) const
{
	ApplyPackedTextureDataToRenderTarget(RenderTarget, this->Data);
}

void UTexturePackerAssetData::ApplyPackedTextureDataToRenderTarget(UTextureRenderTarget2D* RenderTarget, const FTexturePackerInnerData& InData)
{
	RenderTarget->CompressionNoAlpha = !InData.bUseAlpha;
	RenderTarget->ResizeTarget(InData.OutputWidth, InData.OutputHeight);
}

void UTexturePackerAssetData::ApplyPackedTextureDataToMaterial(
	UMaterialInstanceDynamic* Material,
	const FTexturePackerInnerData& InData)
{
	UTexturePackerAssetData::UpdateChannelValue(Material, InData.Red);
	UTexturePackerAssetData::UpdateChannelValue(Material, InData.Green);
	UTexturePackerAssetData::UpdateChannelValue(Material, InData.Blue);

	if (InData.bUseAlpha)
	{
		Material->SetScalarParameterValue("UsingAlpha", 1);
		UTexturePackerAssetData::UpdateChannelValue(Material, InData.Alpha);
	}
	else
	{
		Material->SetScalarParameterValue("UsingAlpha", 0);
		Material->SetScalarParameterValue("UseAlphaTexture", 0);
		Material->SetScalarParameterValue("AlphaChannelInline", 1);
	}
}

void UTexturePackerAssetData::UpdateChannelValue(
	UMaterialInstanceDynamic* Material,
	const FTexturePackerUtilityChannelData& InData)
{
	check(Material);

	switch (InData.Source)
	{
		case ETextureChannelSource::INLINE:
			Material->SetScalarParameterValue(InData.SwitchKey, 0);
			Material->SetScalarParameterValue(InData.InlineKey, InData.SourceInline);
			break;
		case ETextureChannelSource::TEXTURE_RED:
			Material->SetScalarParameterValue(InData.SwitchKey, 1);
			Material->SetScalarParameterValue(InData.SourceKey, 0);
			Material->SetTextureParameterValue(InData.TextureKey, InData.SourceTexture.LoadSynchronous());
			break;
		case ETextureChannelSource::TEXTURE_GREEN:
			Material->SetScalarParameterValue(InData.SwitchKey, 1);
			Material->SetScalarParameterValue(InData.SourceKey, 1);
			Material->SetTextureParameterValue(InData.TextureKey, InData.SourceTexture.LoadSynchronous());
			break;
		case ETextureChannelSource::TEXTURE_BLUE:
			Material->SetScalarParameterValue(InData.SwitchKey, 1);
			Material->SetScalarParameterValue(InData.SourceKey, 2);
			Material->SetTextureParameterValue(InData.TextureKey, InData.SourceTexture.LoadSynchronous());
			break;
		case ETextureChannelSource::TEXTURE_ALPHA:
			Material->SetScalarParameterValue(InData.SwitchKey, 1);
			Material->SetScalarParameterValue(InData.SourceKey, 3);
			Material->SetTextureParameterValue(InData.TextureKey, InData.SourceTexture.LoadSynchronous());
			break;
	}

	if (InData.bSwapUV)
	{
		Material->SetScalarParameterValue(InData.UVSwapKey, 1);
	}
	else
	{
		Material->SetScalarParameterValue(InData.UVSwapKey, 0);
	}
}

void UTexturePackerAssetData::CopyData(const FTexturePackerInnerData& InData)
{
	this->Data = InData;
}
