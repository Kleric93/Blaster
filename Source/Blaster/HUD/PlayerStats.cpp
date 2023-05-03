// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameFramework/PlayerState.h"
#include "Components/ScrollBox.h"
#include "PlayerStatsLine.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"



void UPlayerStats::WidgetSetup(TArray<class ABlasterPlayerState*> BlasterPlayerStateArray)
{
    if (PlayerStatsScrollBox)
    {
        PlayerStatsScrollBox->ClearChildren();
        if (BlasterPlayerStateArray.Num() != 0)
        {
            for (ABlasterPlayerState* BlasterPlayerState : BlasterPlayerStateArray)
            {
                if (BlasterPlayerState == nullptr) continue;

                APlayerState* InGamePlayer = BlasterPlayerState;
                if (InGamePlayer == nullptr) continue;

                PlayerStatsLineWidget = CreateWidget<UPlayerStatsLine>(GetWorld(), PlayerStatLine);
                if (PlayerStatsLineWidget == nullptr) continue;

                PlayerStatsLineWidget->DisplayName->SetText(FText::FromString(InGamePlayer->GetPlayerName()));

                BlasterPlayerState->OnPlayerScoredKill.AddDynamic(this, &UPlayerStats::UpdateKills);

                PlayerStatsLineWidget->AddToViewport();

                PlayerStatsScrollBox->AddChild(PlayerStatsLineWidget);
                UE_LOG(LogTemp, Warning, TEXT("UPlayerStats created the widget setup"));
            }
        }
    }
}

void UPlayerStats::UpdateKills(const FString& PlayerName, int32 NewKills)
{
    // Iterate through the children of PlayerStatsScrollBox to find the entry for the player
    for (int32 i = 0; i < PlayerStatsScrollBox->GetChildrenCount(); ++i)
    {
        UPlayerStatsLine* StatsLine = Cast<UPlayerStatsLine>(PlayerStatsScrollBox->GetChildAt(i));
        if (StatsLine && StatsLine->DisplayName->GetText().ToString() == PlayerName)
        {
            // Update the kills for the player in the scoreboard
            StatsLine->Kills->SetText(FText::AsNumber(NewKills));
            break;
        }
    }
}
