// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"
#include "Components/SphereComponent.h"
#include "Blaster/Pickups/TeamsFlag.h"
#include "Blaster/GameMode/CaptureTheFlagGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"


AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));

	SetRootComponent(ZoneSphere);
}

void AFlagZone::BeginPlay()
{
	Super::BeginPlay();
	
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnSphereOverlap);
}

void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Error, TEXT("Player is overlapping"));

	ATeamsFlag* OverlappingFlag = Cast<ATeamsFlag>(OtherActor);
	bool bCanScoreRedFlag = this->ActorHasTag("BlueZoneTag") && OverlappingFlag && OverlappingFlag->ActorHasTag("RedFlagTag");
	bool bCanScoreBlueFlag = this->ActorHasTag("RedZoneTag") && OverlappingFlag && OverlappingFlag->ActorHasTag("BlueFlagTag");

	if (IsFlagInBase(ETeam::ET_BlueTeam) && bCanScoreRedFlag)
	{
		//UE_LOG(LogTemp, Error, TEXT("OverlappingFlag IS VALID IN THE ZONE"));

		ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
		if (GameMode)
		{
			//UE_LOG(LogTemp, Error, TEXT("Game Mode is valid in overlap zone"));

			GameMode->FlagCaptured(OverlappingFlag, this);
		}

		OverlappingFlag->ServerDetachfromBackpack();

		if (OverlappingFlag)
		{
			//UE_LOG(LogTemp, Error, TEXT("Flag is respawning"));

			OverlappingFlag->MulticastFlagRespawn();
		}
		if (CaptureSound)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				CaptureSound);
		}
	}
	else if (IsFlagInBase(ETeam::ET_RedTeam)&& bCanScoreBlueFlag)
	{
		//UE_LOG(LogTemp, Error, TEXT("OverlappingFlag IS VALID IN THE ZONE"));

		ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
		if (GameMode)
		{
			//UE_LOG(LogTemp, Error, TEXT("Game Mode is valid in overlap zone"));

			GameMode->FlagCaptured(OverlappingFlag, this);
		}

		OverlappingFlag->ServerDetachfromBackpack();

		if (OverlappingFlag)
		{
			//UE_LOG(LogTemp, Error, TEXT("Flag is respawning"));

			OverlappingFlag->MulticastFlagRespawn();
		}
		if (CaptureSound)
		{
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				CaptureSound);
		}
	}
	else
	{
		return;
	}
}

bool AFlagZone::IsFlagInBase(ETeam ScoringTeam) const
{
	FName FlagTag = (ScoringTeam == ETeam::ET_RedTeam) ? "RedFlagTag" : "BlueFlagTag";
	TArray<AActor*> FlagActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FlagTag, FlagActors);
	for (AActor* FlagActor : FlagActors)
	{
		ATeamsFlag* BaseFlag = Cast<ATeamsFlag>(FlagActor);
		if (BaseFlag && BaseFlag->GetTeam() == ScoringTeam && BaseFlag->GetFlagState() == EFlagState::EFS_Initial)
		{
			return true;
		}
	}
	return false;

}



/*
void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATeamsFlag* OverlappingFlag = Cast<ATeamsFlag>(OtherActor);
	bool bCanScoreRedFlag = this->ActorHasTag("BlueZoneTag") && OverlappingFlag && OverlappingFlag->ActorHasTag("RedFlagTag");
	bool bCanScoreBlueFlag = this->ActorHasTag("RedZoneTag") && OverlappingFlag && OverlappingFlag->ActorHasTag("BlueFlagTag");

	if (bCanScoreRedFlag)
	{
		UE_LOG(LogTemp, Error, TEXT("OverlappingFlag BlueTeamShouldScore"));

		FlagScore(OverlappingFlag);
	}
	else if (bCanScoreBlueFlag)
	{
		FlagScore(OverlappingFlag);
	}
}*/




