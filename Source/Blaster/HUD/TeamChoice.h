// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/Team.h"
#include "TeamChoice.generated.h"

class APlayerController;
class ABlasterPlayerController;
class ABlasterGameState;
class UTeamChoiceLine;

class UScrollBox;
class UButton;
class UScrollBox;


/**
 * 
 */
UCLASS()
class BLASTER_API UTeamChoice : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UScrollBox* RedTeamChosenScrollBox;

	UPROPERTY(meta = (BindWidget))
		UScrollBox* BlueTeamChosenScrollBox;

	UPROPERTY(meta = (BindWidget))
		UScrollBox* NoTeamScrollBox;

	UPROPERTY(meta = (BindWidget))
		UButton* RedTeamButton;

	UPROPERTY(meta = (BindWidget))
		UButton* BlueTeamButton;

	UPROPERTY(meta = (BindWidget))
		UButton* CloseButton;

	void WidgetSetup();

	UFUNCTION()
	void InitNoTeamScrollBox();

	void InitRedTeamScrollBox();

	void UpdateScrollBoxes();

	UFUNCTION()
	void WidgetTeardown();

	UFUNCTION()
	void OnRedButtonClicked();

	UFUNCTION()
	void OnBlueButtonClicked();
	

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UTeamChoiceLine> TeamChoiceLine;

	UPROPERTY()
		UTeamChoiceLine* TeamChoiceLineWidget;

};
