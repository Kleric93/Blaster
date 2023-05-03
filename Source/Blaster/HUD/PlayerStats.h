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


	UPROPERTY(meta = (BindWidget))
	class UScrollBox* PlayerStatsScrollBox;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlayerStatsLine> PlayerStatLine;

	UPROPERTY()
	UPlayerStatsLine* PlayerStatsLineWidget;

	UPROPERTY()
		class ABlasterPlayerController* Controller;

	void WidgetSetup(TArray<class ABlasterPlayerState*> BlasterPlayerStateArray);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateKills(const FString& PlayerName, int32 NewKills);
};
