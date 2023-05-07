// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerScoredPoint, const FString&, PlayerName, ETeam, TeamThatScored, int32, PlayerScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamScoredPoint, ETeam, TeamThatScored, int32, TeamScore);


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

	UPROPERTY()
	TArray<ABlasterPlayerController*> PlayerControllersArray;

	UPROPERTY()
	class UPlayerStats* PlayerStats;

	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnPlayerScoredPoint OnPlayerScoredPoint;

	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnTeamScoredPoint OnTeamScoredPoint;

	UPROPERTY()
	TMap<FString, int32> PlayerScores;

private:

	float TopScore = 0.f;
	
};
