// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatsLine.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UPlayerStatsLine : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DisplayName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Kills;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Deaths;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Damage;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* KD;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
		class UImage* TeamIcon;
	
};
