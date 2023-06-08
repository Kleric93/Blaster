// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/HUD/PlayerStats.h"
#include "Kismet/GamePlayStatics.h"
#include "Blaster/HUD/ScoresOverview.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Blaster/BlasterUserSettings.h"


void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
	DOREPLIFETIME_CONDITION_NOTIFY(ABlasterGameState, TimeElapsed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(ABlasterGameState, bMatchEndedAbruptly);
	DOREPLIFETIME(ABlasterGameState, ScoreToWin);
	DOREPLIFETIME(ABlasterGameState, PendingChoicePlayerArray);
	DOREPLIFETIME(ABlasterGameState, RedPlayersArray);
	DOREPLIFETIME(ABlasterGameState, BluePlayersArray);
	DOREPLIFETIME(ABlasterGameState, MaxScore);
}

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
	//UE_LOG(LogTemp, Error, TEXT("Player %s scored a point"), *PlayerName);
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
	//UE_LOG(LogTemp, Error, TEXT("Player %s scored a point"), *PlayerName);
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
	//UE_LOG(LogTemp, Warning, TEXT("GetAllPlayerControllers - PlayerControllersArray size: %d"), PlayerControllersArray.Num());

	// Display the number of player controllers on the screen for all clients
	FString DebugMessage = FString::Printf(TEXT("PlayerControllersArray size: %d"), PlayerControllersArray.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, DebugMessage);

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
	OnPlayerLeft.Broadcast(PlayerLeaving);
}

void ABlasterGameState::Multicast_AddPlayerJoined_Implementation(ABlasterPlayerState* PlayerJoining, const FString& PlayerName)
{
	OnPlayerJoined.Broadcast(PlayerJoining, PlayerName);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Multicast_AddPlayerJoined: PlayerJoined is %s"), *PlayerJoining->GetPlayerName()));
}

void ABlasterGameState::SetFFAVotes()
{
	++TotalFFAVotes;

	Multicast_UpdateFFAVotes(TotalFFAVotes);
}

void ABlasterGameState::SetFFASMVotes()
{
	++TotalFFASMVotes;

	Multicast_UpdateFFASMVotes(TotalFFASMVotes);
}

void ABlasterGameState::SetTDMVotes()
{
	++TotalTDMVotes;

	Multicast_UpdateTDMVotes(TotalTDMVotes);
}

void ABlasterGameState::SetTDMSMVotes()
{
	++TotalTDMSMVotes;

	Multicast_UpdateTDMSMVotes(TotalTDMSMVotes);
}

void ABlasterGameState::SetCTFVotes()
{
	++TotalCTFVotes;

	Multicast_UpdateCTFVotes(TotalCTFVotes);
}

void ABlasterGameState::SetCTFSMVotes()
{
	++TotalCTFSMVotes;

	Multicast_UpdateCTFSMVotes(TotalCTFSMVotes);
}

void ABlasterGameState::SetInstaKillVotes()
{
	++TotalInstaKillVotes;

	Multicast_UpdateInstaKillVotes(TotalInstaKillVotes);
}

void ABlasterGameState::SetInstaKillSMVotes()
{
	++TotalInstaKillSMVotes;

	Multicast_UpdateInstaKillSMVotes(TotalInstaKillSMVotes);
}

void ABlasterGameState::Multicast_UpdateFFAVotes_Implementation(int32 Vote)
{
	OnFFAVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateTDMVotes_Implementation(int32 Vote)
{
	OnTDMVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateCTFVotes_Implementation(int32 Vote)
{
	OnCTFVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateInstaKillVotes_Implementation(int32 Vote)
{
	OnInstaKillVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateFFASMVotes_Implementation(int32 Vote)
{
	OnFFASMVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateTDMSMVotes_Implementation(int32 Vote)
{
	OnTDMSMVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateCTFSMVotes_Implementation(int32 Vote)
{
	OnCTFSMVoteCast.Broadcast(Vote);
}

void ABlasterGameState::Multicast_UpdateInstaKillSMVotes_Implementation(int32 Vote)
{
	OnInstaKillSMVoteCast.Broadcast(Vote);
}


FString ABlasterGameState::CompareVotesAndLog()
{
	// Initialize the maxVote variable and the corresponding gameMode
	int32 maxVote = TotalFFAVotes;
	FString gameMode = "FFA";

	// Check if TDM votes are greater

	if (TotalFFASMVotes > maxVote)
	{
		maxVote = TotalFFASMVotes;
		gameMode = "FFASM";
	}

	if (TotalTDMVotes > maxVote)
	{
		maxVote = TotalTDMVotes;
		gameMode = "TDM";
	}

	if (TotalTDMSMVotes > maxVote)
	{
		maxVote = TotalTDMSMVotes;
		gameMode = "TDMSM";
	}

	// Check if CTF votes are greater
	if (TotalCTFVotes > maxVote)
	{
		maxVote = TotalCTFVotes;
		gameMode = "CTF";
	}

	if (TotalCTFSMVotes > maxVote)
	{
		maxVote = TotalCTFSMVotes;
		gameMode = "CTFSM";
	}

	// Check if InstaKill votes are greater
	if (TotalInstaKillVotes > maxVote)
	{
		maxVote = TotalInstaKillVotes;
		gameMode = "InstaKill";
	}

	if (TotalInstaKillSMVotes > maxVote)
	{
		maxVote = TotalInstaKillSMVotes;
		gameMode = "InstaKillSM";
	}

	// Log the game mode with the highest vote
	//UE_LOG(LogTemp, Warning, TEXT("Game mode with highest vote: %s with %d votes"), *gameMode, maxVote);

	FString mapName;

	// Determine the mapName based on the gameMode with the highest vote
	if (gameMode == "FFA")
	{
		mapName = "/Game/Maps/BlasterMap";
	}
	if (gameMode == "FFASM")
	{
		mapName = "/Game/Maps/BlasterMapSM";
	}
	else if (gameMode == "TDM")
	{
		mapName = "/Game/Maps/Teams";
	}
	else if (gameMode == "TDMSM")
	{
		mapName = "/Game/Maps/TeamsSM";
	}
	else if (gameMode == "CTF")
	{
		mapName = "/Game/Maps/CaptureTheFlag";
	}
	else if (gameMode == "CTFSM")
	{
		mapName = "/Game/Maps/CaptureTheFlagSM";
	}
	else if (gameMode == "InstaKill")
	{
		mapName = "/Game/Maps/InstaKillMap";
	}
	else if (gameMode == "InstaKillSM")
	{
		mapName = "/Game/Maps/InstaKillMapSM";
	}

	// Return the mapName
	return mapName;
}

bool ABlasterGameState::HasMatchEndedAbruptly()
{
	return bMatchEndedAbruptly;
}

void ABlasterGameState::SetHasMatchEndedAbruptly(bool BendedAbruptly)
{
	bMatchEndedAbruptly = BendedAbruptly;
}

void ABlasterGameState::SetTimeElapsed(float TimeElapsedInMatch)
{
	TimeElapsed = TimeElapsedInMatch;
}

void ABlasterGameState::OnRep_TimeElapsed()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(It->Get());
		if (PlayerController)
		{
			PlayerController->SetHUDTime();
		}
	}
}

void ABlasterGameState::OnRep_MatchHasEndedAbruptly()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(It->Get());
		if (PlayerController)
		{
			PlayerController->SetHUDTime();
		}
	}
}

float ABlasterGameState::GetScoreToWinFromServer()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer && BPlayer->Settings && BPlayer->HasAuthority())
	{
		ScoreToWin = BPlayer->Settings->GetMaxScore();
	}
	return ScoreToWin;
}
/* this works just fine, for server though.
float ABlasterGameState::GetScoreToWinFromServer()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer && BPlayer->Settings)
	{
		ScoreToWin = BPlayer->Settings->GetMaxScore();
	}
	return ScoreToWin;
}*/

void ABlasterGameState::ServerFillPendingPlayerStatesArray_Implementation()
{
	MulticastFillPendingPlayerStatesArray();
}

void ABlasterGameState::MulticastFillPendingPlayerStatesArray_Implementation()
{
	PendingChoicePlayerArray.Empty(); // Clear the array before populating

	// Retrieve the game state
	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (GameState)
	{

		// Loop through each player state and add it to the pending choice array
		for (APlayerState* PlayerState : PlayerArray)
		{
			if (PlayerState)
			{
				PendingChoicePlayerArray.Add(PlayerState);
			}
		}
	}
}

void ABlasterGameState::ServerChosenRed_Implementation(APlayerState* PState)
{
	MulticastFillRedPlayerStatesArray(PState);
}

void ABlasterGameState::MulticastFillRedPlayerStatesArray_Implementation(APlayerState* PState)
{
	// Retrieve the game state
	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (GameState)
	{
		// Loop through each player state and add it to the pending choice array
		for (APlayerState* PlayerState : RedPlayersArray)
		{
			if (PlayerState)
			{
				RedPlayersArray.Add(PState);
			}
		}
	}
}



