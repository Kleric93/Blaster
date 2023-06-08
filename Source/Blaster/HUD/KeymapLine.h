// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeymapLine.generated.h"

class UInputKeySelector;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UKeymapLine : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UInputKeySelector* KeyboardKeySelector;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KeyName;
};
