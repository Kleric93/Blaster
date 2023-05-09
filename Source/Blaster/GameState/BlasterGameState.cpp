// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/HUD/PlayerStats.h"
#include "Kismet/GamePlayStatics.h"
#include "Blaster/HUD/ScoresOverview.h"
#include "Blaster/HUD/BlasterHUD.h"

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ABlasterGameState::RedTeamScores(ABlasterPlayerState* ScoringPlayerState)
{
	++RedTeamScore;

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore);
	}

	if (!ScoringPlayerState)
	{
		return;
	}
	ABlasterPlayerController* ScoringPlayerController = Cast<ABlasterPlayerController>(ScoringPlayerState->GetOwner());
	if (!ScoringPlayerController)
	{
		return;
	}

	FString PlayerName = ScoringPlayerState->GetPlayerName();
	UE_LOG(LogTemp, Error, TEXT("Player %s scored a point"), *PlayerName);
	int32& PlayerScore = PlayerScores.FindOrAdd(PlayerName);
	PlayerScore++;
	Multicast_UpdatePlayerScorePoints(PlayerName, ETeam::ET_RedTeam, PlayerScore);
	Multicast_UpdateTeamScorePoints(ETeam::ET_RedTeam, RedTeamScore);
}

void ABlasterGameState::BlueTeamScores(ABlasterPlayerState* ScoringPlayerState)
{
	++BlueTeamScore;

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}

	if (!ScoringPlayerState)
	{
		return;
	}

	ABlasterPlayerController* ScoringPlayerController = Cast<ABlasterPlayerController>(ScoringPlayerState->GetOwner());
	if (!ScoringPlayerController)
	{
		return;
	}

	FString PlayerName = ScoringPlayerState->GetPlayerName();
	UE_LOG(LogTemp, Error, TEXT("Player %s scored a point"), *PlayerName);
	int32& PlayerScore = PlayerScores.FindOrAdd(PlayerName);
	PlayerScore++;
	Multicast_UpdatePlayerScorePoints(PlayerName, ETeam::ET_BlueTeam, PlayerScore);
	Multicast_UpdateTeamScorePoints(ETeam::ET_BlueTeam, BlueTeamScore);
}

void ABlasterGameState::OnRep_RedTeamScore()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void ABlasterGameState::OnRep_BlueTeamScore()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

TArray<class ABlasterPlayerController*> ABlasterGameState::GetAllPlayerControllers()
{
	TArray<ABlasterPlayerController*> Result;
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayerController)
		{
			Result.Add(BlasterPlayerController);
		}
	}

	if (HasAuthority())
	{
		PlayerControllersArray = Result;
	}

	// Log the number of player controllers in the array
	UE_LOG(LogTemp, Warning, TEXT("GetAllPlayerControllers - PlayerControllersArray size: %d"), PlayerControllersArray.Num());

	// Display the number of player controllers on the screen for all clients
	FString DebugMessage = FString::Printf(TEXT("PlayerControllersArray size: %d"), PlayerControllersArray.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, DebugMessage);

	return Result;
}

void ABlasterGameState::Multicast_UpdatePlayerScorePoints_Implementation(const FString& PlayerName, ETeam TeamThatScored, int32 PlayerScore)
{
	OnPlayerScoredPoint.Broadcast(PlayerName, TeamThatScored, PlayerScore);
}

void ABlasterGameState::Multicast_UpdateTeamScorePoints_Implementation(ETeam TeamThatScored, int32 TeamScore)
{
	OnTeamScoredPoint.Broadcast(TeamThatScored, TeamScore);
}

void ABlasterGameState::Multicast_RemovePlayerLeft_Implementation(ABlasterPlayerState* PlayerLeaving)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Multicast_RemovePlayerLeft: PlayerLeaving is %s"), *PlayerLeaving->GetName()));
	OnPlayerLeft.Broadcast(PlayerLeaving);
}

void ABlasterGameState::Multicast_AddPlayerJoined_Implementation(ABlasterPlayerState* PlayerJoining)
{
	OnPlayerJoined.Broadcast(PlayerJoining);
}

