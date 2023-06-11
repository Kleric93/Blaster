// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"
#include "Blaster/BlasterUserSettings.h"

#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/QualitySettingsTypes.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/GameUserSettings.h"
#include "GraphicSettingsLine.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Components/ComboBoxString.h"
#include "Components/InputKeySelector.h"
#include "RHI.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Engine/Engine.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/CheckBox.h"
#include "Components/SpinBox.h"
#include "InputMappingContext.h"
#include "Blaster/InputConfig.h"
#include "Blaster/Input/BlasterGameplayTags.h"


void USettingsMenu::MenuSetup()
{
	if (Settings == nullptr)
	{
		Settings = UBlasterUserSettings::GetBlasterUserSettings();
	}

	AddToViewport(3);
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(QuitButtonAnimation, 0.f, 0);
	bIsFocusable = true;

	ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	Character = Cast<ABlasterCharacter>(PC->GetPawn());
	if (PC)
	{
		PlayerController = PlayerController == nullptr ? GetWorld()->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}


		PC->bReturnToMainMenuOpen = false;
		PC->bSettingsMenuOpen = true;
	}

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	if (GraphicSettingsScrollBox)
	{
		GraphicSettingsScrollBox->ClearChildren();

		for (EGraphicalSettings SettingsLine : TEnumRange<EGraphicalSettings>()) 
		{
			UGraphicSettingsLine* GraphicalSettingsCell = CreateWidget<UGraphicSettingsLine>(World, GraphicalSettingsLine);
			if (GraphicalSettingsCell == nullptr) return;
			GraphicalSettingsCell->Setup(this, SettingsLine);
			GraphicSettingsScrollBox->AddChild(GraphicalSettingsCell);
		}
	}

	if (ResolutionComboBox)
	{
		TArray<FString> Resolutions;
		FScreenResolutionArray ResolutionsArray;
		if (RHIGetAvailableResolutions(ResolutionsArray, true))  // Get only the resolutions that the current display supports.
		{
			for (const FScreenResolutionRHI& Resolution : ResolutionsArray)
			{
				// Convert each resolution to a string and add it to the Resolutions array.
				FString ResolutionStr = FString::Printf(TEXT("%d x %d"), Resolution.Width, Resolution.Height);
				Resolutions.Add(ResolutionStr);
			}
		}

		// Clear the combo box items.
		ResolutionComboBox->ClearOptions();

		// Add the resolution strings to the combo box.
		for (FString& Resolution : Resolutions)
		{
			ResolutionComboBox->AddOption(Resolution);
		}

		// Set the event handler for when the selected option changes.
		ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &USettingsMenu::OnChangeResolution);
	}

	if (FullscreenButton && WindowedButton && FullscreenWindowedButton && QuitButton)
	{
		FullscreenButton->OnClicked.AddDynamic(this, &USettingsMenu::OnFullscreenButtonClicked);
		WindowedButton->OnClicked.AddDynamic(this, &USettingsMenu::OnWindowedButtonClicked);
		FullscreenWindowedButton->OnClicked.AddDynamic(this, &USettingsMenu::OnFullscreenWindowedButtonClicked);
		QuitButton->OnClicked.AddDynamic(this, &USettingsMenu::OnQuitButtonClicked);
	}

	if (Settings->GetFullscreenMode() == EWindowMode::Fullscreen)
	{
		FullscreenButton->SetBackgroundColor(SelectedColor);
		WindowedButton->SetBackgroundColor(NotSelectedColor);
		FullscreenWindowedButton->SetBackgroundColor(NotSelectedColor);
	}
	else if (Settings->GetFullscreenMode() == EWindowMode::Windowed)
	{
		FullscreenButton->SetBackgroundColor(NotSelectedColor);
		WindowedButton->SetBackgroundColor(SelectedColor);
		FullscreenWindowedButton->SetBackgroundColor(NotSelectedColor);
	}
	else if (Settings->GetFullscreenMode() == EWindowMode::WindowedFullscreen)
	{
		FullscreenButton->SetBackgroundColor(NotSelectedColor);
		WindowedButton->SetBackgroundColor(NotSelectedColor);
		FullscreenWindowedButton->SetBackgroundColor(SelectedColor);
	}

	if (MasterVolumeSpinBox)
	{
		MasterVolumeSpinBox->SetValue(CurrentMasterVolume);
		MasterVolumeSpinBox->OnValueChanged.AddDynamic(this, &USettingsMenu::OnMasterSliderValueChanged);
	}
	if (MusicVolumeSpinBox)
	{
		MusicVolumeSpinBox->SetValue(CurrentMusicVolume);
		MusicVolumeSpinBox->OnValueChanged.AddDynamic(this, &USettingsMenu::OnMusicSliderValueChanged);
	}
	if (SFXVolumeSpinBox)
	{
		SFXVolumeSpinBox->SetValue(CurrentSFXVolume);
		SFXVolumeSpinBox->OnValueChanged.AddDynamic(this, &USettingsMenu::OnSFXSliderValueChanged);
	}
	if (VoiceVolumeSpinBox)
	{
		VoiceVolumeSpinBox->SetValue(CurrentVoiceVolume);
		VoiceVolumeSpinBox->OnValueChanged.AddDynamic(this, &USettingsMenu::OnVoiceSliderValueChanged);
	}
	if (Character == nullptr) return;
	if (AimSpeedMultiplierSlider && Character && Settings)
	{
		//AimSpeedMultiplierSlider->SetValue(Settings->GetBaseTurnRate());
		AimSpeedMultiplierSlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnAimSpeedMultiplierSliderValueChanged);
	}

	if (AimedRotationSpeedLZSlider && Character && Settings)
	{
		//AimedRotationSpeedLZSlider->SetValue(Settings->GetAimingTurnRateForLongZoom());
		AimedRotationSpeedLZSlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnAimedRotationSpeedLZSliderValueChanged);
	}

	if (AimedRotationSpeedSZSlider && Character && Settings)
	{
		//AimedRotationSpeedSZSlider->SetValue(Settings->GetAimingTurnRateForShortZoom());
		AimedRotationSpeedSZSlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnAimedRotationSpeedSZSliderValueChanged);
	}

	if (OverheadWidgetVisibilityCheckBox)
	{
		OverheadWidgetVisibilityCheckBox->SetIsChecked(Settings->GetbLocalPlayerOverheadWidgetVisibility());
		OverheadWidgetVisibilityCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsMenu::OnOverheadWidgetVisibilityCheckBoxStatusChange);
	}

	if (Settings->GetIsUsingKBM() == true)
	{
		MousenKeyboardInputButton->SetIsEnabled(false);
		ControllerInputButton->SetIsEnabled(true);
	}
	else if (Settings->GetIsUsingKBM() == false)
	{
		MousenKeyboardInputButton->SetIsEnabled(true);
		ControllerInputButton->SetIsEnabled(false);
	}
	if (MousenKeyboardInputButton)
	{
		MousenKeyboardInputButton->OnPressed.AddDynamic(this, &USettingsMenu::OnMousenKeyboardInputButtonClicked);
	}

	if (ControllerInputButton)
	{
		ControllerInputButton->OnPressed.AddDynamic(this, &USettingsMenu::OnControllerInputButtonClicked);
	}
}

void USettingsMenu::OnMousenKeyboardInputButtonClicked()
{
	ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->IMCSelector(PC->GetKBMMappingContext(), PC->GetControllerMappingContext());
	}
	MousenKeyboardInputButton->SetIsEnabled(false);
	ControllerInputButton->SetIsEnabled(true);
	Settings->SetIsUsingKBM(true);
	Settings->SaveSettings();
}

void USettingsMenu::OnControllerInputButtonClicked()
{
	ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->IMCSelector(PC->GetControllerMappingContext(), PC->GetKBMMappingContext());
	}
	MousenKeyboardInputButton->SetIsEnabled(true);
	ControllerInputButton->SetIsEnabled(false);
	Settings->SetIsUsingKBM(false);
	Settings->SaveSettings();
}

void USettingsMenu::OnFullscreenButtonClicked()
{
	Settings->SetFullscreenMode(EWindowMode::Fullscreen);
	Settings->ApplySettings(true);
	
	FullscreenButton->SetBackgroundColor(SelectedColor);
	WindowedButton->SetBackgroundColor(NotSelectedColor);
	FullscreenWindowedButton->SetBackgroundColor(NotSelectedColor);
}

void USettingsMenu::OnWindowedButtonClicked()
{
	Settings->SetFullscreenMode(EWindowMode::Windowed);
	Settings->ApplySettings(true);

	FullscreenButton->SetBackgroundColor(NotSelectedColor);
	WindowedButton->SetBackgroundColor(SelectedColor);
	FullscreenWindowedButton->SetBackgroundColor(NotSelectedColor);
}

void USettingsMenu::OnFullscreenWindowedButtonClicked()
{
	Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	Settings->ApplySettings(true);

	FullscreenButton->SetBackgroundColor(NotSelectedColor);
	WindowedButton->SetBackgroundColor(NotSelectedColor);
	FullscreenWindowedButton->SetBackgroundColor(SelectedColor);
}

void USettingsMenu::OnQuitButtonClicked()
{
	MenuTearDown();
}

void USettingsMenu::MenuTearDown()
{
	RemoveFromParent();

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}

	ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->bReturnToMainMenuOpen = false;
		PC->bSettingsMenuOpen = false;
		UE_LOG(LogTemp, Error, TEXT("MenuTeardown got inside the PC"))
	}
	GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}


void USettingsMenu::NativeDestruct()
{
	MenuTearDown();
	UUserWidget::NativeDestruct();
}

void USettingsMenu::OnChangeResolution(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// Split the selected item string to get the width and height.
	TArray<FString> ResolutionArray;
	SelectedItem.ParseIntoArray(ResolutionArray, TEXT("x"), true);

	// Convert the width and height strings to integers.
	int32 Width = FCString::Atoi(*ResolutionArray[0]);
	int32 Height = FCString::Atoi(*ResolutionArray[1]);

	// Create an FIntPoint object from the width and height.
	FIntPoint Resolution = FIntPoint(Width, Height);

	// Set the screen resolution.
	if (GEngine)
	{
		UGameUserSettings* UserSettings = GEngine->GameUserSettings;
		if (UserSettings)
		{
			UserSettings->SetScreenResolution(Resolution);
			UserSettings->ApplyResolutionSettings(false);
			UserSettings->ApplySettings(true);
		}
	}
}

void USettingsMenu::SetGraphicalSettings(EGraphicalSettings GraphSetting, uint32 QualityValue)
{
	if (!GEngine && !GEngine->GameUserSettings) { return; }

	switch (GraphSetting) 
	
	 {
	case EGraphicalSettings::EGS_View_Distance:		 GEngine->GameUserSettings->SetViewDistanceQuality(QualityValue);		break;
	case EGraphicalSettings::EGS_Anti_Aliasing:		 GEngine->GameUserSettings->SetAntiAliasingQuality(QualityValue);		break;
	case EGraphicalSettings::EGS_Post_Processing:	 GEngine->GameUserSettings->SetPostProcessingQuality(QualityValue);		break;
	case EGraphicalSettings::EGS_Shadows:			 GEngine->GameUserSettings->SetShadowQuality(QualityValue);				break;
	case EGraphicalSettings::EGS_Global_Illumination:GEngine->GameUserSettings->SetGlobalIlluminationQuality(QualityValue);	break;
	case EGraphicalSettings::EGS_Reflection:		 GEngine->GameUserSettings->SetReflectionQuality(QualityValue);			break;
	case EGraphicalSettings::EGS_Textures:			 GEngine->GameUserSettings->SetTextureQuality(QualityValue);			break;
	case EGraphicalSettings::EGS_Effects:			 GEngine->GameUserSettings->SetVisualEffectQuality(QualityValue);		break;
	case EGraphicalSettings::EGS_Foliage:			 GEngine->GameUserSettings->SetFoliageQuality(QualityValue);			break;
	case EGraphicalSettings::EGS_Shading:			 GEngine->GameUserSettings->SetShadingQuality(QualityValue);			break;
	default: break;
	}

	GEngine->GameUserSettings->ApplySettings(false);
}

void USettingsMenu::OnMasterSliderValueChanged(float Value)
{
	if (MasterSoundClass)
	{
		MasterSoundClass->Properties.Volume = Value;
		CurrentMasterVolume = Value;  // Store the volume level
	}
}

void USettingsMenu::OnMusicSliderValueChanged(float Value)
{
	if (MusicSoundClass)
	{
		MusicSoundClass->Properties.Volume = Value;
		CurrentMusicVolume = Value;
	}
}

void USettingsMenu::OnSFXSliderValueChanged(float Value)
{
	if (SFXSoundClass)
	{
		SFXSoundClass->Properties.Volume = Value;
		CurrentSFXVolume = Value;
	}
}

void USettingsMenu::OnVoiceSliderValueChanged(float Value)
{
	if (VoiceSoundClass)
	{
		VoiceSoundClass->Properties.Volume = Value;
		CurrentVoiceVolume = Value;
	}
}

void USettingsMenu::OnAimSpeedMultiplierSliderValueChanged(float Value)
{
	if (AimSpeedMultiplierSlider)
	{
		Settings->SetBaseTurnRate(Value);
		Settings->SaveSettings();
	}
}

void USettingsMenu::OnAimedRotationSpeedLZSliderValueChanged(float Value)
{
	if (AimedRotationSpeedLZSlider)
	{
		Settings->SetAimingTurnRateForLongZoom(Value);
		Settings->SaveSettings();
	}
}

void USettingsMenu::OnAimedRotationSpeedSZSliderValueChanged(float Value)
{
	if (AimedRotationSpeedSZSlider)
	{
		Settings->SetAimingTurnRateForShortZoom(Value);
		Settings->SaveSettings();
	}
}

void USettingsMenu::OnOverheadWidgetVisibilityCheckBoxStatusChange(bool IsOHBoxChecked)
{
	if (OverheadWidgetVisibilityCheckBox)
	{
		Settings->SetbLocalPlayerOverheadWidgetVisibility(IsOHBoxChecked);
		Settings->SaveSettings();
	}
}

