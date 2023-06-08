// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerScoredPoint, const FString&, PlayerName, ETeam, TeamThatScored, int32, PlayerScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamScoredPoint, ETeam, TeamThatScored, int32, TeamScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLeftDelegate, ABlasterPlayerState*, PlayerLeaving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerJoinedDelegate, ABlasterPlayerState*, PlayerJoining, const FString&, PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFFAVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTDMVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCTFVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInstaKillVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFFASMVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTDMSMVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCTFSMVoteCast, int32, Vote);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInstaKillSMVoteCast, int32, Vote);



UENUM(BlueprintType)
enum class EStatTypes : uint8
{
	EST_Score UMETA(DisplayName = "Score"),
	EST_Kills UMETA(DisplayName = "Kills"),
	EST_Deaths UMETA(DisplayName = "Deaths"),
	EST_KD UMETA(DisplayName = "KD"),
};


/**
 *
 */
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ABlasterPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
		TArray<class ABlasterPlayerState*> TopScoringPlayers;

	UPROPERTY()
	TMap<EStatTypes, float> PlayerStatistics;

	UPROPERTY(Replicated)
	TArray<APlayerState*> PendingChoicePlayerArray;

	UPROPERTY(Replicated)
	TArray<APlayerState*> RedPlayersArray;
	UPROPERTY(Replicated)
	TArray<APlayerState*> BluePlayersArray;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float MaxScore;

	//
	/// Teams
	//


	void RedTeamScores(ABlasterPlayerState* ScoringPlayerState);
	void BlueTeamScores(ABlasterPlayerState* ScoringPlayerState);

	UFUNCTION(BlueprintPure)
	float GetScoreToWinFromServer();

	UFUNCTION(Server, Reliable)
	void ServerChosenRed(APlayerState* PState);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFillRedPlayerStatesArray(APlayerState* PState);

	UFUNCTION(Server, Reliable)
	void ServerFillPendingPlayerStatesArray();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFillPendingPlayerStatesArray();

	TArray<ABlasterPlayerState*> RedTeam;
	TArray<ABlasterPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
		float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
		float BlueTeamScore = 0.f;

	UFUNCTION()
		void OnRep_RedTeamScore();

	UFUNCTION()
		void OnRep_BlueTeamScore();

	UFUNCTION(BlueprintCallable, Category = "PlayerControllers")
		TArray<class ABlasterPlayerController*> GetAllPlayerControllers();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdatePlayerScorePoints(const FString& PlayerName, ETeam TeamThatScored, int32 PlayerScore);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateTeamScorePoints(ETeam TeamThatScored, int32 TeamScore);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_RemovePlayerLeft(ABlasterPlayerState* PlayerLeaving);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_AddPlayerJoined(ABlasterPlayerState* PlayerJoining, const FString& PlayerName);

	UPROPERTY()
		TArray<ABlasterPlayerController*> PlayerControllersArray;

	UPROPERTY()
		class UPlayerStats* PlayerStats;

	UPROPERTY(BlueprintAssignable, Category = "Score")
		FOnPlayerScoredPoint OnPlayerScoredPoint;

	UPROPERTY(BlueprintAssignable, Category = "Score")
		FOnTeamScoredPoint OnTeamScoredPoint;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FPlayerLeftDelegate OnPlayerLeft;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FPlayerJoinedDelegate OnPlayerJoined;

	UPROPERTY()
		TMap<FString, int32> PlayerScores;

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UVotingSyastem> VotingSystemWidget;

	UPROPERTY()
		class UVotingSyastem* VotingSystem;


	UPROPERTY(Replicated)
		int32 TotalFFAVotes;

	UPROPERTY(Replicated)
		int32 TotalFFASMVotes;

	UPROPERTY(Replicated)
		int32 TotalTDMVotes;

	UPROPERTY(Replicated)
		int32 TotalTDMSMVotes;

	UPROPERTY(Replicated)
		int32 TotalCTFVotes;

	UPROPERTY(Replicated)
		int32 TotalCTFSMVotes;

	UPROPERTY(Replicated)
		int32 TotalInstaKillVotes;

	UPROPERTY(Replicated)
		int32 TotalInstaKillSMVotes;

	UFUNCTION()
		void SetFFAVotes();

	UFUNCTION()
		void SetFFASMVotes();

	UFUNCTION()
		void SetTDMVotes();

	UFUNCTION()
		void SetTDMSMVotes();

	UFUNCTION()
		void SetCTFVotes();

	UFUNCTION()
		void SetCTFSMVotes();

	UFUNCTION()
		void SetInstaKillVotes();

	UFUNCTION()
		void SetInstaKillSMVotes();

	UFUNCTION(BlueprintPure)
	FString CompareVotesAndLog();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateFFAVotes(int32 Vote);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateFFASMVotes(int32 Vote);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateTDMVotes(int32 Vote);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateTDMSMVotes(int32 Vote);


	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateCTFVotes(int32 Vote);


	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateCTFSMVotes(int32 Vote);


	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateInstaKillVotes(int32 Vote);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateInstaKillSMVotes(int32 Vote);

	bool HasMatchEndedAbruptly();

	void SetHasMatchEndedAbruptly(bool BendedAbruptly); 
	void SetTimeElapsed(float TimeElapsedInMatch);
	float GetTimeElapsed() const { return TimeElapsed; }

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnFFAVoteCast OnFFAVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnFFASMVoteCast OnFFASMVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnTDMVoteCast OnTDMVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnTDMSMVoteCast OnTDMSMVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnCTFVoteCast OnCTFVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnCTFSMVoteCast OnCTFSMVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnInstaKillVoteCast OnInstaKillVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnInstaKillSMVoteCast OnInstaKillSMVoteCast;

	float TopScore = 0.f;

protected:

		UFUNCTION()
			void OnRep_TimeElapsed();

		UFUNCTION()
			void OnRep_MatchHasEndedAbruptly();

private:

	UPROPERTY(ReplicatedUsing = OnRep_MatchHasEndedAbruptly)
	bool bMatchEndedAbruptly = false;

	UPROPERTY(ReplicatedUsing = OnRep_TimeElapsed)
		float TimeElapsed;

	UPROPERTY(Replicated)
		float ScoreToWin;
};