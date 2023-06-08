// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "TeamsGameMode.generated.h"

class UBlasterUserSettings;
class ABlasterPlayerController;
class ABlasterGameState;

/**
 * 
 */
UCLASS()
class BLASTER_API ATeamsGameMode : public ABlasterGameMode
{
	GENERATED_BODY()
	
public:
	ATeamsGameMode();
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	void OnTeamChosen(ABlasterPlayerController* BPController, ETeam ChosenTeam);

	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;


protected:

	virtual void HandleMatchHasStarted() override;

private:

	ABlasterPlayerController* BlasterController;
	ABlasterGameState* BlasterGameState;
};
