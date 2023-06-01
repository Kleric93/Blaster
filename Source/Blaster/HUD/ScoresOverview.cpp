// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoresOverview.h"
#include "PlayerStats.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/GameMode/CaptureTheFlagGameMode.h"
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
                TArray<ABlasterPlayerState*> PlayerStatesArray;
                for (APlayerState* PlayerState : MyGameState->PlayerArray)
                {
                    BlasterPlayerState = Cast<ABlasterPlayerState>(PlayerState);
                    if (BlasterPlayerState)
                    {
                        PlayerStatesArray.Add(BlasterPlayerState);
                    }
                }

                if (BlasterPlayerState->GetTeam() == ETeam::ET_NoTeam)
                {
                    PlayerStats->WidgetSetup(PlayerStatesArray);
                }
                else
                {
                    PlayerStats->WidgetSetupTeams(PlayerStatesArray);
                }

                PlayerStats->AddToViewport(3);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("scrollbox is null in ScoresOverview"));
    }
}
