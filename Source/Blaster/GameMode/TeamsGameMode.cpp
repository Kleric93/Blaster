// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterUserSettings.h"

ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	BlasterController = Cast<ABlasterPlayerController>(NewPlayer);
	BlasterGameState = Cast <ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = NewPlayer->GetPlayerState<ABlasterPlayerState>();

	Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
	if (BlasterGameState)
	{
		if (BlasterController)
		{
			BlasterController->OnTeamChosen.AddDynamic(this, &ATeamsGameMode::OnTeamChosen);
			if (BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				BlasterGameState->ServerFillPendingPlayerStatesArray();

				if (BlasterGameState->BlueTeam.Num() >= BlasterGameState->RedTeam.Num())
				{
					BlasterGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BlasterGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

void ATeamsGameMode::OnTeamChosen(ABlasterPlayerController* BPController, ETeam ChosenTeam)
{
	ABlasterGameState* BGameState = Cast <ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(BPController->PlayerState);
	if (BPState && BPController)
	{
		if (ChosenTeam == ETeam::ET_RedTeam)
		{
			BGameState->RedTeam.AddUnique(BPState);
			BPState->SetTeam(ETeam::ET_RedTeam);
			
		}
		else if (ChosenTeam == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeam.AddUnique(BPState);
			BPState->SetTeam(ETeam::ET_BlueTeam);
		}
	
	}
	//GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	ABlasterGameState* BGameState = Cast <ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = Exiting->GetPlayerState<ABlasterPlayerState>();
	if (BGameState && BPState)
	{
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
		BGameState->Multicast_RemovePlayerLeft(BPState);
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ABlasterGameState* BGameState = Cast <ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PState : BGameState->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get());
			
			if (BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	if (!Attacker || !Victim) return BaseDamage;

	ABlasterPlayerState* AttackerPState = Attacker ? Attacker->GetPlayerState<ABlasterPlayerState>() : nullptr;
	ABlasterPlayerState* VictimPState = Victim ? Victim->GetPlayerState<ABlasterPlayerState>() : nullptr;

	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;

	if (VictimPState == AttackerPState) return BaseDamage;

	if (AttackerPState->GetTeam() == VictimPState->GetTeam()) return 0.f;

	return BaseDamage;
}


void ATeamsGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	ABlasterGameState* BGameState = Cast <ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;

	if (BGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores(AttackerPlayerState);
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores(AttackerPlayerState);
		}
	}
}

