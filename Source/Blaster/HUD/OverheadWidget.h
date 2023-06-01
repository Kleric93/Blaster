// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/Team.h"
#include "OverheadWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText;

	UPROPERTY(meta = (BindWidget))
	UImage* OverheadWidgetBorder;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

	void ChangeOWColor(ETeam Team);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetTextSize(int32 Size);


protected:

	void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);
	
};
