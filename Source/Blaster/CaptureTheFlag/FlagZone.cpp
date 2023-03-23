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

	if (OverlappingFlag && OverlappingFlag->GetTeam() != Team)
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



