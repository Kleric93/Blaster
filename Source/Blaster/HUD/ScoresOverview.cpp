// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoresOverview.h"
#include "PlayerStats.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ScrollBox.h"


void UScoresOverview::StatsSetup()
{
	if (TeamsDataScrollbox)
	{
		PlayerStats = CreateWidget<UPlayerStats>(GetWorld(), PlayerStatsWidget);

		if (PlayerStats != nullptr)
		{
			ABlasterGameState* MyGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));

			if (MyGameState != nullptr)
			{
				PlayerStats->WidgetSetup(MyGameState->PlayerArray);
				PlayerStats->AddToViewport();

				UE_LOG(LogTemp, Warning, TEXT("UScoresOverview created the scrollbox"));
				GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("scrollbox is null in ScoresOverview"));

	}
}

void UScoresOverview::StatsTeardown()
{
}
