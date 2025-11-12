#include "EditorUtilityWidgets/TexturePackerUtilityWidget.h"

UTexturePackerUtilityWidget::UTexturePackerUtilityWidget()
{
	Red.SwitchKey  = "UseRedTexture";
	Red.InlineKey  = "RedChannelInline";
	Red.TextureKey = "RedChannel";
	Red.SourceKey  = "RedChannelSource";
	Red.UVSwapKey  = "RedSwapUV";

	Green.SwitchKey  = "UseGreenTexture";
	Green.InlineKey  = "GreenChannelInline";
	Green.TextureKey = "GreenChannel";
	Green.SourceKey  = "GreenChannelSource";
	Green.UVSwapKey  = "GreenSwapUV";

	Blue.SwitchKey  = "UseBlueTexture";
	Blue.InlineKey  = "BlueChannelInline";
	Blue.TextureKey = "BlueChannel";
	Blue.SourceKey  = "BlueChannelSource";
	Blue.UVSwapKey  = "BlueSwapUV";

	Alpha.SwitchKey  = "UseAlphaTexture";
	Alpha.InlineKey  = "AlphaChannelInline";
	Alpha.TextureKey = "AlphaChannel";
	Alpha.SourceKey  = "AlphaChannelSource";
	Alpha.UVSwapKey  = "AlphaSwapUV";
}

void UTexturePackerUtilityWidget::UpdatePreviewDisplay()
{
	auto Preview = this->GetDisplayMaterial();
	check(Preview);

	this->UpdateChannelValue(Preview, Red);
	this->UpdateChannelValue(Preview, Green);
	this->UpdateChannelValue(Preview, Blue);

	if (this->bUseAlpha)
	{
		this->UpdateChannelValue(Preview, Alpha);
	}
	else
	{
		Preview->SetScalarParameterValue("UseAlphaTexture", 0);
		Preview->SetScalarParameterValue("AlphaChannelInline", 1);
	}
}

void UTexturePackerUtilityWidget::UpdateChannelValue(
	UMaterialInstanceDynamic* Material,
	const FTexturePackerUtilityChannelData& Data)
{
	check(Material);

	switch (Data.Source)
	{
		case ETextureChannelSource::INLINE:
			Material->SetScalarParameterValue(Data.SwitchKey, 0);
			Material->SetScalarParameterValue(Data.InlineKey, Data.SourceInline);
			break;
		case ETextureChannelSource::TEXTURE_RED:
			Material->SetScalarParameterValue(Data.SwitchKey, 1);
			Material->SetScalarParameterValue(Data.SourceKey, 0);
			Material->SetTextureParameterValue(Data.TextureKey, Data.SourceTexture);
			break;
		case ETextureChannelSource::TEXTURE_GREEN:
			Material->SetScalarParameterValue(Data.SwitchKey, 1);
			Material->SetScalarParameterValue(Data.SourceKey, 1);
			Material->SetTextureParameterValue(Data.TextureKey, Data.SourceTexture);
			break;
		case ETextureChannelSource::TEXTURE_BLUE:
			Material->SetScalarParameterValue(Data.SwitchKey, 1);
			Material->SetScalarParameterValue(Data.SourceKey, 2);
			Material->SetTextureParameterValue(Data.TextureKey, Data.SourceTexture);
			break;
		case ETextureChannelSource::TEXTURE_ALPHA:
			Material->SetScalarParameterValue(Data.SwitchKey, 1);
			Material->SetScalarParameterValue(Data.SourceKey, 3);
			Material->SetTextureParameterValue(Data.TextureKey, Data.SourceTexture);
			break;
	}

	if (Data.bSwapUV)
	{
		Material->SetScalarParameterValue(Data.UVSwapKey, 1);
	}
	else
	{
		Material->SetScalarParameterValue(Data.UVSwapKey, 0);
	}
}