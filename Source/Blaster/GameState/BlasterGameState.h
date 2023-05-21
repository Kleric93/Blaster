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

	//
	/// Teams
	//

	void RedTeamScores(ABlasterPlayerState* ScoringPlayerState);
	void BlueTeamScores(ABlasterPlayerState* ScoringPlayerState);

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


	UPROPERTY(ReplicatedUsing = OnRep_FFATotalVotes)
		int32 TotalFFAVotes;

	UPROPERTY(Replicated)
		int32 TotalTDMVotes;

	UPROPERTY(Replicated)
		int32 TotalCTFVotes;

	UPROPERTY(Replicated)
		int32 TotalInstaKillVotes;

	UFUNCTION()
		void OnRep_FFATotalVotes();

	UFUNCTION()
		void SetFFAVotes();

	UFUNCTION()
		void SetTDMVotes();

	UFUNCTION()
		void SetCTFVotes();

	UFUNCTION()
		void SetInstaKillVotes();

	FString CompareVotesAndLog();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateFFAVotes(int32 Vote);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateTDMVotes(int32 Vote);


	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateCTFVotes(int32 Vote);


	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateInstaKillVotes(int32 Vote);


	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnFFAVoteCast OnFFAVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnTDMVoteCast OnTDMVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnCTFVoteCast OnCTFVoteCast;

	UPROPERTY(BlueprintAssignable, Category = "Voting")
		FOnInstaKillVoteCast OnInstaKillVoteCast;
private:

	float TopScore = 0.f;

};