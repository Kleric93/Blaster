// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamChoice.generated.h"

class APlayerController;
class ABlasterPlayerController;
class ABlasterGameState;

class UScrollBox;
class UButton;


/**
 * 
 */
UCLASS()
class BLASTER_API UTeamChoice : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		UButton* RedTeamButton;

	UPROPERTY(meta = (BindWidget))
		UButton* BlueTeamButton;

	UPROPERTY(meta = (BindWidget))
		UButton* CloseButton;

	void WidgetSetup();

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
};
