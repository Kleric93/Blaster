// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, Team);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
			FString PlayerName = this->GetPlayerName();
			int32 NewKills = FMath::FloorToInt(Score);
			int32 NewDeaths = Defeats;
			Multicast_UpdatePlayerKills(PlayerName, NewKills);
			Multicast_UpdatePlayerKD(PlayerName, NewKills, NewDeaths);

			//UE_LOG(LogTemp, Error, TEXT("Server_UpdatePlayerKills_Implementation called for player: %s, kills: %d"), *PlayerName, NewKills);
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
			FString PlayerName = this->GetPlayerName();
			int32 NewDeaths = Defeats;
			int32 NewKills = GetScore();
			Multicast_UpdatePlayerDeaths(PlayerName, NewDeaths);
			Multicast_UpdatePlayerKD(PlayerName, NewKills, NewDeaths);

			//UE_LOG(LogTemp, Error, TEXT("Server_UpdatePlayerDeaths_Implementation called for player: %s, Deaths: %d"), *PlayerName, NewDeaths);
		}
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;
	ABlasterCharacter* BCharacter = Cast<ABlasterCharacter>(GetPawn());
	UE_LOG(LogTemp, Error, TEXT("SetTeam called for player %s with team %d"), *GetPlayerName(), static_cast<uint8>(TeamToSet));
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	TimerDelegate.BindUFunction(this, FName("DelayedMulticastUpdateTeam"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 11.0f, false);

	GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void ABlasterPlayerState::DelayedMulticastUpdateTeam()
{
	FString PlayerName = this->GetPlayerName();
	Multicast_UpdateTeam(PlayerName, Team);
}

void ABlasterPlayerState::OnRep_Team()
{
	ABlasterCharacter* BCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
}

void ABlasterPlayerState::Multicast_UpdatePlayerKills_Implementation(const FString& PlayerName, int32 NewKills)
{
	OnPlayerScoredKill.Broadcast(PlayerName, NewKills);
}

void ABlasterPlayerState::Multicast_UpdatePlayerDeaths_Implementation(const FString& PlayerName, int32 NewDeaths)
{
	OnPlayerDeath.Broadcast(PlayerName, NewDeaths);
}

void ABlasterPlayerState::Multicast_UpdatePlayerKD_Implementation(const FString& PlayerName, int32 NewKills, int32 NewDeaths)
{
	OnPlayerKDUpdated.Broadcast(PlayerName, NewKills, NewDeaths);
}

void ABlasterPlayerState::Multicast_UpdateTeam_Implementation(const FString& PlayerName, ETeam TeamAssigned)
{
	OnPlayerTeamAssigned.Broadcast(PlayerName, TeamAssigned);
}