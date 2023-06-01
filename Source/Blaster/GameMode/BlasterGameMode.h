// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class UBlasterUserSettings;

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer
}

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ABlasterGameMode();

	UPROPERTY()
	UBlasterUserSettings* Settings;

	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class ABlasterPlayerState* PlayerLeaving);

	void SendChat(const FString& Text, const FString& PlayerName);

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;

	UPROPERTY(EditAnywhere);
	float MatchTime;

	bool bCaptureTheFlagMatch = false;

	UPROPERTY()
	class UVotingSyastem* VotingSystem;

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 0.f;

private:
	UPROPERTY(EditDefaultsOnly)
	float NearbyRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector DefaultLocation = FVector(0, 0, 0);

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }

};
