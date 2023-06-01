// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"
#include "Components/SphereComponent.h"
#include "Blaster/Pickups/TeamsFlag.h"
#include "Blaster/GameMode/CaptureTheFlagGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"


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
    if (!OtherActor)
    {
        return;
    }

    ATeamsFlag* OverlappingFlag = Cast<ATeamsFlag>(OtherActor);
    if (!OverlappingFlag)
    {
        return;
    }
    bool bCanScoreRedFlag = this->ActorHasTag("BlueZoneTag") && OverlappingFlag && OverlappingFlag->ActorHasTag("RedFlagTag");
    bool bCanScoreBlueFlag = this->ActorHasTag("RedZoneTag") && OverlappingFlag && OverlappingFlag->ActorHasTag("BlueFlagTag");

    if (IsFlagInBase(ETeam::ET_BlueTeam) && bCanScoreRedFlag)
    {
        ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
        if (GameMode)
        {
            ABlasterCharacter* Character = Cast<ABlasterCharacter>(OverlappingFlag->GetOwner());
            ABlasterPlayerState* BPlayerState = Cast<ABlasterPlayerState>(Character->GetPlayerState());

            if (BPlayerState)
            {
                GameMode->FlagCaptured(OverlappingFlag, this, BPlayerState);
            }
        }

        OverlappingFlag->ServerDetachfromBackpack();

        if (OverlappingFlag)
        {
            OverlappingFlag->MulticastFlagRespawn();
        }
        if (CaptureSound)
        {
            UGameplayStatics::PlaySound2D(
                GetWorld(),
                CaptureSound);
        }
    }
    else if (IsFlagInBase(ETeam::ET_RedTeam) && bCanScoreBlueFlag)
    {
        ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
        if (GameMode)
        {
            ABlasterCharacter* Character = Cast<ABlasterCharacter>(OverlappingFlag->GetOwner());
            ABlasterPlayerState* BPlayerState = Cast<ABlasterPlayerState>(Character->GetPlayerState());

            if (BPlayerState)
            {
                GameMode->FlagCaptured(OverlappingFlag, this, BPlayerState);
            }
        }

        OverlappingFlag->ServerDetachfromBackpack();

        if (OverlappingFlag)
        {
            OverlappingFlag->MulticastFlagRespawn();
        }
        if (CaptureSound)
        {
            UGameplayStatics::PlaySound2D(
                GetWorld(),
                CaptureSound);
        }
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



