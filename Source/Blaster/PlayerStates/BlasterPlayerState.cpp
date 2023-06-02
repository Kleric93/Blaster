// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/HUD/VotingSyastem.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Pickups/HealthPickup.h"
#include "Blaster/Pickups/ShieldPickup.h"
#include "Blaster/Pickups/SpeedPickup.h"
#include "Blaster/Pickups/JumpPickup.h"
#include "Blaster/Pickups/BerserkPickup.h"
#include "Blaster/Pickups/PickupSpawnPoint.h"
#include "Blaster/BlasterUserSettings.h"
#include "EngineUtils.h"


ABlasterPlayerState::ABlasterPlayerState()
{
}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, Team);
	DOREPLIFETIME(ABlasterPlayerState, TeamChoice);

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
	if (BCharacter)
	{
		BCharacter->SetTeamColor(Team);
	}
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	APlayerController* BaseController = this->GetPlayerController();
	ABlasterPlayerController* BPController = Cast<ABlasterPlayerController>(BaseController);
	float TimerforBeginPlay = BPController->GetWarmupTime() + 0.5f;
	if (BPController)
	{
		TimerDelegate.BindUFunction(this, FName("DelayedMulticastUpdateTeam"));
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, TimerforBeginPlay, false);
	}

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

void ABlasterPlayerState::RegisterBuffSpawnPoints()
{
	for (TActorIterator<APickupSpawnPoint> It(GetWorld()); It; ++It)
	{
		It->OnSpeedBuffSpawned.AddDynamic(this, &ABlasterPlayerState::OnBuffSpawned);
		It->OnJumpBuffSpawned.AddDynamic(this, &ABlasterPlayerState::OnBuffSpawned);
		It->OnBerserkBuffSpawned.AddDynamic(this, &ABlasterPlayerState::OnBuffSpawned);
		It->OnHealthBuffSpawned.AddDynamic(this, &ABlasterPlayerState::OnBuffSpawned);
		It->OnShieldBuffSpawned.AddDynamic(this, &ABlasterPlayerState::OnBuffSpawned);
	}
}

void ABlasterPlayerState::OnBuffSpawned(APickupSpawnPoint* SpawnPoint)
{
	if (SpawnPoint && SpawnPoint->SpawnedPickup)
	{
		// Check if the spawned pickup is a speed pickup
		ASpeedPickup* SpeedPickup = Cast<ASpeedPickup>(SpawnPoint->SpawnedPickup);
		AJumpPickup* JumpPickup = Cast<AJumpPickup>(SpawnPoint->SpawnedPickup);
		ABerserkPickup* BerserkPickup = Cast<ABerserkPickup>(SpawnPoint->SpawnedPickup);
		AHealthPickup* HealthPickup = Cast<AHealthPickup>(SpawnPoint->SpawnedPickup);
		AShieldPickup* ShieldPickup = Cast<AShieldPickup>(SpawnPoint->SpawnedPickup);

		if (SpeedPickup)
		{
			SpeedPickup->OnSpeedBuffPickedUp.AddDynamic(this, &ABlasterPlayerState::OnSpeedBuffPickedUp);
		}
		if (JumpPickup)
		{
			JumpPickup->OnJumpBuffPickedUp.AddDynamic(this, &ABlasterPlayerState::OnJumpBuffPickedUp);
		}
		if (BerserkPickup)
		{
			BerserkPickup->OnBerserkBuffPickedUp.AddDynamic(this, &ABlasterPlayerState::OnBerserkBuffPickedUp);
		}
		if (HealthPickup)
		{
			HealthPickup->OnHealthBuffPickedUp.AddDynamic(this, &ABlasterPlayerState::OnHealthBuffPickedUp);
		}
		if (ShieldPickup)
		{
			ShieldPickup->OnShieldBuffPickedUp.AddDynamic(this, &ABlasterPlayerState::OnShieldBuffPickedUp);
		}
	}
}

void ABlasterPlayerState::OnSpeedBuffPickedUp(float BuffTime)
{
	if (Controller)
	{
		Controller->UpdateSpeedBuffIcon(true);
		Controller->EventBorderPowerUp();
		GetWorldTimerManager().SetTimer(TimerHandle_SpeedBuffDuration, this, &ABlasterPlayerState::OnSpeedBuffEnd, BuffTime, false);
	}
}

void ABlasterPlayerState::OnSpeedBuffEnd()
{
	if (Controller)
	{
		Controller->UpdateSpeedBuffIcon(false);
	}
}


void ABlasterPlayerState::OnJumpBuffPickedUp(float BuffTime)
{
	if (Controller)
	{
		Controller->UpdateJumpBuffIcon(true);
		Controller->EventBorderPowerUp();
		GetWorldTimerManager().SetTimer(TimerHandle_JumpBuffDuration, this, &ABlasterPlayerState::OnJumpBuffEnd, BuffTime, false);
	}
}

void ABlasterPlayerState::OnJumpBuffEnd()
{
	if (Controller)
	{
		Controller->UpdateJumpBuffIcon(false);
	}
}

void ABlasterPlayerState::OnBerserkBuffPickedUp(float BuffTime)
{
	if (Controller)
	{
		Controller->UpdateBerserkBuffIcon(true);
		Controller->EventBorderPowerUp();
		GetWorldTimerManager().SetTimer(TimerHandle_BerserkBuffDuration, this, &ABlasterPlayerState::OnBerserkBuffEnd, BuffTime, false);
	}
}

void ABlasterPlayerState::OnBerserkBuffEnd()
{
	if (Controller)
	{
		Controller->UpdateBerserkBuffIcon(false);
	}
}

void ABlasterPlayerState::OnHealthBuffPickedUp()
{
	if (Controller)
	{
		Controller->EventBorderHeal();
	}
}

void ABlasterPlayerState::OnShieldBuffPickedUp()
{
	if (Controller)
	{
		Controller->EventBorderShield();
	}
}

void ABlasterPlayerState::ServerSetTeamChoice_Implementation()
{
	Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
	if (Settings)
	{
		TeamChoice = Settings->GetTeamToChoose();
	}
	
	UE_LOG(LogTemp, Error, TEXT("ServerSetTeamChoice_Implementation with team"));
}

void ABlasterPlayerState::OnRep_TeamChoice()
{
	SetTeam(TeamChoice);
}
