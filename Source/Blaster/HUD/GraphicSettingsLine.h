// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/QualitySettingsTypes.h"
#include "GraphicSettingsLine.generated.h"

class UTextBlock;
class UButton;
struct FLinearColor;

/**
 * 
 */
UCLASS()
class BLASTER_API UGraphicSettingsLine : public UUserWidget
{
	GENERATED_BODY()

public:

	void Setup(class USettingsMenu* InParent, EGraphicalSettings InSettingsParam);
	
	void UdpateButtonsColor();

	void UpdateIndividualButtonColor(UButton* InButton, ESettingsLevel buttonLevel);

	void MenuTearDown();

	FLinearColor SelectedColor{ 0.0f, 0.6f , 1.0f , 1.0f };					// cyan
	FLinearColor NotSelectedColor{ 0.1f, 1.0f , 0.0f , 1.0f };				// green

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SettingText;

	UPROPERTY(meta = (BindWidget))
		UButton* LowButton;

	UPROPERTY(meta = (BindWidget))
		UButton* MediumButton;

	UPROPERTY(meta = (BindWidget))
		UButton* HighButton;

	UPROPERTY(meta = (BindWidget))
		UButton* EpicButton;

	UPROPERTY(meta = (BindWidget))
		UButton* CinemaButton;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* LowText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MediumText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* HighText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* EpicText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* CinemaText;


	UFUNCTION()
		void LowButtonClick();

	UFUNCTION()
		void MediumButtonClick();

	UFUNCTION()
		void HighButtonClick();

	UFUNCTION()
		void EpicButtonClick();

	UFUNCTION()
		void CinemaButtonClick();

	UPROPERTY()
	class USettingsMenu* Parent;

	UPROPERTY(BlueprintReadOnly)
		bool bIsSelected = false;

	EGraphicalSettings DefinedParameter;

	UPROPERTY(BlueprintReadOnly)
	ESettingsLevel CurrentSelectedLevel = ESettingsLevel::ESL_Mid;

	int32 QualityValue = 2;

protected:

	virtual bool Initialize() override;
};
