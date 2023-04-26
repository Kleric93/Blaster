// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameFramework/PlayerState.h"
#include "Components/ScrollBox.h"
#include "PlayerStatsLine.h"


void UPlayerStats::WidgetSetup(TArray<APlayerState*> PlayerStatesArray)
{
	if (PlayerStatsScrollBox)
	{
		PlayerStatsScrollBox->ClearChildren();
		if (PlayerStatesArray.Num() != 0)
		{
			for (APlayerState* InGamePlayer : PlayerStatesArray)
			{
				if (InGamePlayer == nullptr) return;

				UPlayerStatsLine* PlayerLine = CreateWidget<UPlayerStatsLine>(GetWorld(), PlayerStatLine);
				if (PlayerLine == nullptr) return;

				PlayerLine->DisplayName->SetText(FText::FromString(InGamePlayer->GetPlayerName()));
				PlayerLine->AddToViewport();

				PlayerStatsScrollBox->AddChild(PlayerLine);
				UE_LOG(LogTemp, Warning, TEXT("UPlayerStats created the widget setup"));

			}
		}
	}
}
