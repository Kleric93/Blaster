// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStats.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UPlayerStats : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void WidgetSetup(TArray<APlayerState*> PlayerStatesArray);

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* PlayerStatsScrollBox;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlayerStatsLine> PlayerStatLine;
};
