// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphicSettingsLine.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Math/Color.h"
#include "Blaster/HUD/SettingsMenu.h"
#include "GameFramework/GameUserSettings.h"

void UGraphicSettingsLine::Setup(USettingsMenu* InParent, EGraphicalSettings InSettingsParam)
{
	if (!GEngine && !GEngine->GameUserSettings) { return; }

	Parent = InParent;
	DefinedParameter = InSettingsParam;
	if (SettingText)
	{
		switch (DefinedParameter)
		{

		case EGraphicalSettings::EGS_View_Distance:
			SettingText->SetText(SPT_View_Distance);
			QualityValue = GEngine->GameUserSettings->GetViewDistanceQuality();
			break;
		case EGraphicalSettings::EGS_Anti_Aliasing:
			SettingText->SetText(SPT_Anti_Aliasing);
			QualityValue = GEngine->GameUserSettings->GetAntiAliasingQuality();
			break;
		case EGraphicalSettings::EGS_Post_Processing:
			SettingText->SetText(SPT_Post_Processing);
			QualityValue = GEngine->GameUserSettings->GetPostProcessingQuality();
			break;
		case EGraphicalSettings::EGS_Shadows:
			SettingText->SetText(SPT_Shadows);
			QualityValue = GEngine->GameUserSettings->GetShadowQuality();
			break;
		case EGraphicalSettings::EGS_Global_Illumination:
			SettingText->SetText(SPT_Global_Illumination);
			QualityValue = GEngine->GameUserSettings->GetGlobalIlluminationQuality();
			break;
		case EGraphicalSettings::EGS_Reflection:
			SettingText->SetText(SPT_Reflection);
			QualityValue = GEngine->GameUserSettings->GetReflectionQuality();
			break;
		case EGraphicalSettings::EGS_Textures:
			SettingText->SetText(SPT_Textures);
			QualityValue = GEngine->GameUserSettings->GetTextureQuality();
			break;
		case EGraphicalSettings::EGS_Effects:
			SettingText->SetText(SPT_Effects);
			QualityValue = GEngine->GameUserSettings->GetVisualEffectQuality();
			break;
		case EGraphicalSettings::EGS_Foliage:
			SettingText->SetText(SPT_Foliage);
			QualityValue = GEngine->GameUserSettings->GetFoliageQuality();
			break;
		case EGraphicalSettings::EGS_Shading:
			SettingText->SetText(SPT_Shading);
			QualityValue = GEngine->GameUserSettings->GetShadingQuality();
			break;
		default:	break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SettingsText is NULLPTR"));
	}
	


	switch (QualityValue) 
	{
	case 0:	CurrentSelectedLevel = ESettingsLevel::ESL_Low;		break;
	case 1:	CurrentSelectedLevel = ESettingsLevel::ESL_Mid;		break;
	case 2:	CurrentSelectedLevel = ESettingsLevel::ESL_High;	break;
	case 3:	CurrentSelectedLevel = ESettingsLevel::ESL_Epic;	break;
	case 4:	CurrentSelectedLevel = ESettingsLevel::ESL_Cinema;	break;
	default:CurrentSelectedLevel = ESettingsLevel::ESL_Mid;		break;
	}

	if (LowButton && !LowButton->OnClicked.IsBound()) {
		LowButton->OnClicked.AddDynamic(this, &UGraphicSettingsLine::LowButtonClick);
	}

	if (MediumButton && !MediumButton->OnClicked.IsBound()) {
		MediumButton->OnClicked.AddDynamic(this, &UGraphicSettingsLine::MediumButtonClick);
	}

	if (HighButton && !HighButton->OnClicked.IsBound()) {
		HighButton->OnClicked.AddDynamic(this, &UGraphicSettingsLine::HighButtonClick);
	}

	if (EpicButton && !EpicButton->OnClicked.IsBound()) {
		EpicButton->OnClicked.AddDynamic(this, &UGraphicSettingsLine::EpicButtonClick);
	}

	if (CinemaButton && !CinemaButton->OnClicked.IsBound()) {
		CinemaButton->OnClicked.AddDynamic(this, &UGraphicSettingsLine::CinemaButtonClick);
	}

	UdpateButtonsColor();
}

bool UGraphicSettingsLine::Initialize()
{
	if (!UUserWidget::Initialize()) { return false; }

	return true;
}

void UGraphicSettingsLine::MenuTearDown()
{
	if (LowButton && LowButton->OnClicked.IsBound()) {
		LowButton->OnClicked.RemoveDynamic(this, &UGraphicSettingsLine::LowButtonClick);
	}

	if (MediumButton && MediumButton->OnClicked.IsBound()) {
		MediumButton->OnClicked.RemoveDynamic(this, &UGraphicSettingsLine::MediumButtonClick);
	}

	if (HighButton && HighButton->OnClicked.IsBound()) {
		HighButton->OnClicked.RemoveDynamic(this, &UGraphicSettingsLine::HighButtonClick);
	}

	if (EpicButton && EpicButton->OnClicked.IsBound()) {
		EpicButton->OnClicked.RemoveDynamic(this, &UGraphicSettingsLine::EpicButtonClick);
	}

	if (CinemaButton && CinemaButton->OnClicked.IsBound()) {
		CinemaButton->OnClicked.RemoveDynamic(this, &UGraphicSettingsLine::CinemaButtonClick);
	}
}

void UGraphicSettingsLine::UdpateButtonsColor()
{
	UpdateIndividualButtonColor(LowButton, ESettingsLevel::ESL_Low);
	UpdateIndividualButtonColor(MediumButton, ESettingsLevel::ESL_Mid);
	UpdateIndividualButtonColor(HighButton, ESettingsLevel::ESL_High);
	UpdateIndividualButtonColor(EpicButton, ESettingsLevel::ESL_Epic);
	UpdateIndividualButtonColor(CinemaButton, ESettingsLevel::ESL_Cinema);
}

void UGraphicSettingsLine::UpdateIndividualButtonColor(UButton* InButton, ESettingsLevel InLevel)
{
	if (InButton)
	{
		if (InLevel == CurrentSelectedLevel) 
		{
			InButton->SetBackgroundColor(SelectedColor);
		}
		else 
		{
			InButton->SetBackgroundColor(NotSelectedColor);
		}
	}
}

void UGraphicSettingsLine::LowButtonClick()
{
	CurrentSelectedLevel = ESettingsLevel::ESL_Low;
	QualityValue = 0;
	UdpateButtonsColor();
	Parent->SetGraphicalSettings(DefinedParameter, QualityValue);
}

void UGraphicSettingsLine::MediumButtonClick()
{
	CurrentSelectedLevel = ESettingsLevel::ESL_Mid;
	QualityValue = 1;
	UdpateButtonsColor();
	Parent->SetGraphicalSettings(DefinedParameter, QualityValue);
}

void UGraphicSettingsLine::HighButtonClick()
{
	CurrentSelectedLevel = ESettingsLevel::ESL_High;
	QualityValue = 2;
	UdpateButtonsColor();
	Parent->SetGraphicalSettings(DefinedParameter, QualityValue);
}

void UGraphicSettingsLine::EpicButtonClick()
{
	CurrentSelectedLevel = ESettingsLevel::ESL_Epic;
	QualityValue = 3;
	UdpateButtonsColor();
	Parent->SetGraphicalSettings(DefinedParameter, QualityValue);
}

void UGraphicSettingsLine::CinemaButtonClick()
{
	CurrentSelectedLevel = ESettingsLevel::ESL_Cinema;
	QualityValue = 4;
	UdpateButtonsColor();
	Parent->SetGraphicalSettings(DefinedParameter, QualityValue);
}