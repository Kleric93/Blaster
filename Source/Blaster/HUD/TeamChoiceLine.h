// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamChoiceLine.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class BLASTER_API UTeamChoiceLine : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Playername;

	UPROPERTY(meta = (BindWidget))
	UImage* PlayerNameBorder;
	
};
