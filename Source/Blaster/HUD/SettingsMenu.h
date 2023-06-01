// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/QualitySettingsTypes.h"
#include "SettingsMenu.generated.h"

class UButton;
class UWidgetSwitcher;
class UWidget;
class UUserWidget;
class UEditableTextBox;
class USlider;
class UGraphicSettingsLine;
class UScrollBox;
class APlayerController;
class UComboBoxString;
class USoundClass;
class USoundMix;
class ABlasterCharacter;
class UBlasterUserSettings;
class UCheckBox;
class USpinBox;

/**
 * 
 */
UCLASS()
class BLASTER_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void MenuSetup();
	void MenuTearDown();

	void SetGraphicalSettings(EGraphicalSettings GraphSetting, uint32 QualityValue);


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* ResolutionComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundClass* MasterSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundClass* MusicSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundClass* SFXSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundClass* VoiceSoundClass;


protected:

	UBlasterUserSettings* Settings;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnChangeResolution(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnFullscreenWindowedButtonClicked();

	UFUNCTION()
		void OnWindowedButtonClicked();

	UFUNCTION()
		void OnFullscreenButtonClicked();

	UFUNCTION()
		void OnQuitButtonClicked();


	UFUNCTION()
	void OnMasterSliderValueChanged(float Value);

	UFUNCTION()
	void OnMusicSliderValueChanged(float Value);

	UFUNCTION()
	void OnSFXSliderValueChanged(float Value);

	UFUNCTION()
	void OnVoiceSliderValueChanged(float Value);

	UFUNCTION()
	void OnAimSpeedMultiplierSliderValueChanged(float Value);

	UFUNCTION()
		void OnAimedRotationSpeedLZSliderValueChanged(float Value);

	UFUNCTION()
		void OnAimedRotationSpeedSZSliderValueChanged(float Value);

	UFUNCTION()
	void OnOverheadWidgetVisibilityCheckBoxStatusChange(bool IsOHBoxChecked);


private:

	FLinearColor SelectedColor{ 0.0f, 0.6f , 1.0f , 1.0f };					// cyan
	FLinearColor NotSelectedColor{ 0.1f, 1.0f , 0.0f , 1.0f };				// green

	UPROPERTY(meta = (BindWidget))
	UScrollBox* GraphicSettingsScrollBox;

	UPROPERTY(meta = (BindWidget))
	UButton* WindowedButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FullscreenButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FullscreenWindowedButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		USpinBox* MasterVolumeSpinBox;

	UPROPERTY()
		float CurrentMasterVolume;

	UPROPERTY(meta = (BindWidget))
		USpinBox* MusicVolumeSpinBox;

	UPROPERTY()
		float CurrentMusicVolume;

	UPROPERTY(meta = (BindWidget))
		USpinBox* SFXVolumeSpinBox;

	UPROPERTY()
		float CurrentSFXVolume;

	UPROPERTY(meta = (BindWidget))
		USpinBox* VoiceVolumeSpinBox;

	UPROPERTY()
		float CurrentVoiceVolume;

	UPROPERTY(meta = (BindWidget))
		USpinBox* AimSpeedMultiplierSlider;

	UPROPERTY(meta = (BindWidget))
		USpinBox* AimedRotationSpeedLZSlider;

	UPROPERTY(meta = (BindWidget))
		USpinBox* AimedRotationSpeedSZSlider;

	UPROPERTY(meta = (BindWidget))
		UCheckBox* OverheadWidgetVisibilityCheckBox;
	


	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GraphicalSettingsLine;

	UPROPERTY()
	UGraphicSettingsLine* GraphicalSettingsLines;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
		ABlasterCharacter* Character;

public:

};
