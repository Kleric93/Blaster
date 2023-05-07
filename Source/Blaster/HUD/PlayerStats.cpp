// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameFramework/PlayerState.h"
#include "Components/ScrollBox.h"
#include "PlayerStatsLine.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/Weapon/HitScanWeapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Kismet/GameplayStatics.h"


void UPlayerStats::WidgetSetup(TArray<class ABlasterPlayerState*> BlasterPlayerStateArray)
{
    RedTeamScore->SetText(FText::FromString(""));
    BlueTeamScore->SetText(FText::FromString(""));

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
                SetupPlayerStatsLineWidget(BlasterPlayerState, InGamePlayer);


                PlayerStatsScrollBox->AddChild(PlayerStatsLineWidget);

                PlayerStatsLineWidget->AddToViewport();

                UE_LOG(LogTemp, Warning, TEXT("UPlayerStats created the widget setup"));
            }
        }
    }
}

void UPlayerStats::WidgetSetupTeams(TArray<class ABlasterPlayerState*> BlasterPlayerStateArray)
{
    if (PlayerStatsScrollBox_RedTeam)
    {
        PlayerStatsScrollBox_RedTeam->ClearChildren();
    }
    if (PlayerStatsScrollBox_BlueTeam)
    {
        PlayerStatsScrollBox_BlueTeam->ClearChildren();
    }

    RedTeamScore->SetText(FText::FromString("0"));
    BlueTeamScore->SetText(FText::FromString("0"));

    if (BlasterPlayerStateArray.Num() > 0)
    {
        for (ABlasterPlayerState* BlasterPlayerState : BlasterPlayerStateArray)
        {
            if (BlasterPlayerState == nullptr) continue;

            APlayerState* InGamePlayer = BlasterPlayerState;
            if (InGamePlayer == nullptr) continue;

            PlayerStatsLineWidget = CreateWidget<UPlayerStatsLine>(GetWorld(), PlayerStatLine);
            if (PlayerStatsLineWidget == nullptr) continue;

            SetupPlayerStatsLineWidget(BlasterPlayerState, InGamePlayer);

            PlayerStatsLineWidget->AddToViewport();

            if (BlasterPlayerState->GetTeam() == ETeam::ET_RedTeam && PlayerStatsScrollBox_RedTeam)
            {
                PlayerStatsScrollBox_RedTeam->AddChild(PlayerStatsLineWidget);
            }
            else if (BlasterPlayerState->GetTeam() == ETeam::ET_BlueTeam && PlayerStatsScrollBox_BlueTeam)
            {
                PlayerStatsScrollBox_BlueTeam->AddChild(PlayerStatsLineWidget);
            }
            else
            {
                continue;
            }


            UE_LOG(LogTemp, Warning, TEXT("UPlayerStats created the widget setup"));
        }
    }
}

void UPlayerStats::SetupPlayerStatsLineWidget(ABlasterPlayerState* BlasterPlayerState, APlayerState* InGamePlayer)
{
    ABlasterGameState* GameState = Cast<ABlasterGameState>(GetWorld()->GetGameState());
    if (!GameState)
    {
        GameState = Cast<ABlasterGameState>(GetWorld()->GetGameState());

    }

    PlayerStatsLineWidget->DisplayName->SetText(FText::FromString(InGamePlayer->GetPlayerName()));

    BlasterPlayerState->OnPlayerScoredKill.AddDynamic(this, &UPlayerStats::UpdateKills);
    BlasterPlayerState->OnPlayerDeath.AddDynamic(this, &UPlayerStats::UpdateDeaths);
    BlasterPlayerState->OnPlayerKDUpdated.AddDynamic(this, &UPlayerStats::UpdateKD);
    BlasterPlayerState->OnPlayerTeamAssigned.AddDynamic(this, &UPlayerStats::UpdateTeam);

    if (GameState)
    {
        GameState->OnPlayerScoredPoint.AddDynamic(this, &UPlayerStats::UpdateTeamScorePoints);
        GameState->OnTeamScoredPoint.AddDynamic(this, &UPlayerStats::UpdateOverallScores);
    }   
}


UPlayerStatsLine* UPlayerStats::FindPlayerStatsLine(const FString& PlayerName)
{
    TArray<UScrollBox*> ScrollBoxes = { PlayerStatsScrollBox, PlayerStatsScrollBox_RedTeam, PlayerStatsScrollBox_BlueTeam };
    for (UScrollBox* ScrollBox : ScrollBoxes)
    {
        if (!ScrollBox) continue;

        for (int32 i = 0; i < ScrollBox->GetChildrenCount(); ++i)
        {
            UPlayerStatsLine* StatsLine = Cast<UPlayerStatsLine>(ScrollBox->GetChildAt(i));
            if (StatsLine && StatsLine->DisplayName->GetText().ToString() == PlayerName)
            {
                return StatsLine;
            }
        }
    }

    return nullptr;
}

void UPlayerStats::UpdateKills(const FString& PlayerName, int32 NewKills)
{
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);
    if (StatsLine)
    {
        StatsLine->Kills->SetText(FText::AsNumber(NewKills));
    }
}

void UPlayerStats::UpdateDeaths(const FString& PlayerName, int32 NewDeaths)
{
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);
    if (StatsLine)
    {
        StatsLine->Deaths->SetText(FText::AsNumber(NewDeaths));
    }
}

void UPlayerStats::UpdateKD(const FString& PlayerName, int32 Kills, int32 Deaths)
{
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);
    if (StatsLine)
    {
        float KD = (Deaths == 0) ? static_cast<float>(Kills) : static_cast<float>(Kills) / static_cast<float>(Deaths);
        FString KDString = FString::Printf(TEXT("%.1f"), KD);
        StatsLine->KD->SetText(FText::FromString(KDString));
    }
}

void UPlayerStats::UpdateTeam(const FString& PlayerName, ETeam TeamToAssign)
{
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);
    if (StatsLine)
    {
        ETeam Team = TeamToAssign;

        if (Team == ETeam::ET_NoTeam)
        {
            StatsLine->TeamIcon->SetBrushFromTexture(NoTeamIcon);
            StatsLine->DisplayName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        }
        if (Team == ETeam::ET_RedTeam)
        {
            StatsLine->TeamIcon->SetBrushFromTexture(RedTeamIcon);
            StatsLine->DisplayName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            StatsLine->IconBorder->SetColorAndOpacity(FLinearColor::Red);
            StatsLine->NameBorder->SetColorAndOpacity(FLinearColor::Red);
            StatsLine->ScoreBorder->SetColorAndOpacity(FLinearColor::Red);
            StatsLine->KillsBorder->SetColorAndOpacity(FLinearColor::Red);
            StatsLine->DeathsBorder->SetColorAndOpacity(FLinearColor::Red);
            StatsLine->KDBorder->SetColorAndOpacity(FLinearColor::Red);

        }
        if (Team == ETeam::ET_BlueTeam)
        {
            StatsLine->TeamIcon->SetBrushFromTexture(BlueTeamIcon);
            StatsLine->DisplayName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            StatsLine->IconBorder->SetColorAndOpacity(FLinearColor::Blue);
            StatsLine->NameBorder->SetColorAndOpacity(FLinearColor::Blue);
            StatsLine->ScoreBorder->SetColorAndOpacity(FLinearColor::Blue);
            StatsLine->KillsBorder->SetColorAndOpacity(FLinearColor::Blue);
            StatsLine->DeathsBorder->SetColorAndOpacity(FLinearColor::Blue);
            StatsLine->KDBorder->SetColorAndOpacity(FLinearColor::Blue);
        }
    }
}

void UPlayerStats::UpdateTeamScorePoints(const FString& PlayerName, ETeam TeamThatScored, int32 PlayerScore)
{
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);
    if (StatsLine)
    {
        StatsLine->ScoreText->SetText(FText::AsNumber(PlayerScore));
    }
}

void UPlayerStats::UpdateOverallScores(ETeam TeamThatScored, int32 PointScored)
{
    ETeam ScoringTeam = TeamThatScored;

    if (ScoringTeam == ETeam::ET_RedTeam)
    {
        RedTeamScore->SetText(FText::AsNumber(PointScored));
    }
    if (ScoringTeam == ETeam::ET_BlueTeam)
    {
        BlueTeamScore->SetText(FText::AsNumber(PointScored));
    }
}