// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ServerListLine.generated.h"

class UButton;
class UTextBlock;


/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UServerListLine : public UUserWidget
{
	GENERATED_BODY()

public:

	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(meta = (BindWidget))
		UButton* ServerJoin;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerNameText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* HostNameText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MatchTypeText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* ServerPingText;
};
