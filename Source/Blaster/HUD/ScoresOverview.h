// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoresOverview.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UScoresOverview : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void StatsSetup();
	void StatsTeardown();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerStats> PlayerStatsWidget;

	UPROPERTY()
	UPlayerStats* PlayerStats;

protected:

	FTimerHandle SetupTimerHandle;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;


private:

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* TeamsDataScrollbox;
};
