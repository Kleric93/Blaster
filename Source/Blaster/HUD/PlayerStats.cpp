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

                //UE_LOG(LogTemp, Warning, TEXT("UPlayerStats created the widget setup"));
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


            //UE_LOG(LogTemp, Warning, TEXT("UPlayerStats created the widget setup"));
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
        GameState->OnPlayerLeft.AddDynamic(this, &UPlayerStats::RemovePlayerFromScoreboard);
        GameState->OnPlayerJoined.AddDynamic(this, &UPlayerStats::AddPlayerToScoreboard);
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

bool UPlayerStats::IsLocalPlayer(const FString& PlayerName)
{
    APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (LocalPlayerController && LocalPlayerController->PlayerState)
    {
        return LocalPlayerController->PlayerState->GetPlayerName() == PlayerName;
    }

    return false;
}


void UPlayerStats::UpdateTeam(const FString& PlayerName, ETeam TeamToAssign)
{
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);
    if (StatsLine)
    {
        ETeam Team = TeamToAssign;
        bool bIsLocalPlayer = IsLocalPlayer(PlayerName);

        FLinearColor BorderColor;
        FLinearColor CyanColor(0.0f, 1.0f, 1.0f, 1.0f);

        if (bIsLocalPlayer)
        {
            BorderColor = FLinearColor::Green;
        }
        else
        {
            if (Team == ETeam::ET_RedTeam)
            {
                BorderColor = FLinearColor::Red;
            }
            else if (Team == ETeam::ET_BlueTeam)
            {
                BorderColor = FLinearColor::Blue;
            }
            else if (Team == ETeam::ET_NoTeam)
            {
                BorderColor = CyanColor;
            }
        }

        if (Team == ETeam::ET_NoTeam)
        {
            StatsLine->TeamIcon->SetBrushFromTexture(NoTeamIcon);
            StatsLine->DisplayName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        }
        if (Team == ETeam::ET_RedTeam)
        {
            StatsLine->TeamIcon->SetBrushFromTexture(RedTeamIcon);
            StatsLine->DisplayName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        }
        if (Team == ETeam::ET_BlueTeam)
        {
            StatsLine->TeamIcon->SetBrushFromTexture(BlueTeamIcon);
            StatsLine->DisplayName->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        }

        StatsLine->IconBorder->SetColorAndOpacity(BorderColor);
        StatsLine->NameBorder->SetColorAndOpacity(BorderColor);
        StatsLine->ScoreBorder->SetColorAndOpacity(BorderColor);
        StatsLine->KillsBorder->SetColorAndOpacity(BorderColor);
        StatsLine->DeathsBorder->SetColorAndOpacity(BorderColor);
        StatsLine->KDBorder->SetColorAndOpacity(BorderColor);
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

void UPlayerStats::RemovePlayerFromScoreboard(ABlasterPlayerState* PlayerLeaving)
{
    if (!PlayerLeaving) return;

    FString PlayerName = PlayerLeaving->GetPlayerName();
    UPlayerStatsLine* StatsLine = FindPlayerStatsLine(PlayerName);

    if (StatsLine)
    {
        StatsLine->RemoveFromParent();
    }
}

void UPlayerStats::AddPlayerToScoreboard(ABlasterPlayerState* PlayerJoining)
{
    if (!PlayerJoining) return;

    // Check if player is already in the scoreboard
    if (FindPlayerStatsLine(PlayerJoining->GetPlayerName()))
    {
        return;
    }

    // Check if player joining is the server
    if (PlayerJoining->HasAuthority())
    {
        return;
    }

    UPlayerStatsLine* NewStatsLine = CreateWidget<UPlayerStatsLine>(GetWorld(), PlayerStatLine);
    if (NewStatsLine)
    {
        APlayerState* InGamePlayer = PlayerJoining;
        if (InGamePlayer == nullptr) return;

        NewStatsLine->DisplayName->SetText(FText::FromString(InGamePlayer->GetPlayerName()));
        SetupPlayerStatsLineWidget(PlayerJoining, InGamePlayer);

        NewStatsLine->AddToViewport();

        ETeam PlayerTeam = PlayerJoining->GetTeam();
        if (PlayerTeam == ETeam::ET_RedTeam && PlayerStatsScrollBox_RedTeam)
        {
            PlayerStatsScrollBox_RedTeam->AddChild(NewStatsLine);
        }
        else if (PlayerTeam == ETeam::ET_BlueTeam && PlayerStatsScrollBox_BlueTeam)
        {
            PlayerStatsScrollBox_BlueTeam->AddChild(NewStatsLine);
        }
        else
        {
            PlayerStatsScrollBox->AddChild(NewStatsLine);
        }
    }
}



