// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"


class UBlasterUserSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerScoredKill, const FString&, PlayerName, int32, NewKills);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerDeath, const FString&, PlayerName, int32, NewDeaths);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerKDUpdated, const FString&, PlayerName, int32, Kills, int32, Deaths);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerTeamAssigned, const FString&, PlayerName, ETeam, Team);

/**
 *
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABlasterPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//
	/// Replication notifies
	//

	virtual void OnRep_Score() override;

	UFUNCTION()
		virtual void OnRep_Defeats();

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	UFUNCTION()
		void DelayedMulticastUpdateTeam();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdatePlayerKills(const FString& PlayerName, int32 NewKills);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdatePlayerDeaths(const FString& PlayerName, int32 NewDeaths);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdatePlayerKD(const FString& PlayerName, int32 NewKills, int32 NewDeaths);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateTeam(const FString& PlayerName, ETeam TeamAssigned);

	//speed buff functions for Icon update

	UFUNCTION()
		void OnBuffSpawned(APickupSpawnPoint* SpawnPoint);

	UFUNCTION()
		void OnSpeedBuffPickedUp(float BuffTime);

	UFUNCTION()
		void OnSpeedBuffEnd();



	//Jump buff functions for Icon update

	UFUNCTION()
		void OnJumpBuffPickedUp(float BuffTime);

	UFUNCTION()
		void OnJumpBuffEnd();


	//Berserk buff functions for Icon update

	UFUNCTION()
		void OnBerserkBuffPickedUp(float BuffTime);

	UFUNCTION()
		void OnBerserkBuffEnd();

	void RegisterBuffSpawnPoints();



	UPROPERTY(BlueprintAssignable, Category = "Score")
		FOnPlayerScoredKill OnPlayerScoredKill;

	UPROPERTY(BlueprintAssignable, Category = "Score")
		FOnPlayerDeath OnPlayerDeath;

	UPROPERTY(BlueprintAssignable, Category = "Score")
		FOnPlayerKDUpdated OnPlayerKDUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Score")
		FOnPlayerTeamAssigned 	OnPlayerTeamAssigned;

	UPROPERTY(ReplicatedUsing = OnRep_TeamChoice)
	ETeam TeamChoice = ETeam::ET_NoTeam;

private:

	UPROPERTY()
		class ABlasterCharacter* Character;

	UPROPERTY()
		class ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
		int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
		ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
		void OnRep_Team();

	UFUNCTION()
		void OnRep_TeamChoice();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSetTeamChoice();


	FTimerHandle TimerHandle_SpeedBuffDuration;

	FTimerHandle TimerHandle_JumpBuffDuration;

	FTimerHandle TimerHandle_BerserkBuffDuration;

	UPROPERTY()
	UBlasterUserSettings* Settings;

public:

	void SetTeam(ETeam TeamToSet);
	FORCEINLINE	ETeam GetTeam() const { return Team; }
};