// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagRespawner.h"
#include "Components/BoxComponent.h"
#include "Blaster/Pickups/TeamsFlag.h"


AFlagRespawner::AFlagRespawner()
{
	PrimaryActorTick.bCanEverTick = false;

    RespawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FlagRespawnBox"));

    SetRootComponent(RespawnBox);
}

void AFlagRespawner::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
    bool bRespawnFlag = OverlappingFlag->ActorHasTag("RedFlagTag") || OverlappingFlag->ActorHasTag("BlueFlagTag");;

    if (bRespawnFlag)
    {
        OverlappingFlag->ServerDetachfromBackpack();

        if (OverlappingFlag)
        {
            OverlappingFlag->MulticastFlagRespawn();
        }
    }
}

void AFlagRespawner::BeginPlay()
{
	Super::BeginPlay();

    RespawnBox->OnComponentBeginOverlap.AddDynamic(this, &AFlagRespawner::OnComponentBeginOverlap);

}

void AFlagRespawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

