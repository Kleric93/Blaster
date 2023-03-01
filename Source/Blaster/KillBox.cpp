// Fill out your copyright notice in the Description page of Project Settings.


#include "KillBox.h"
#include "Components/BoxComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/Controller.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"


AKillBox::AKillBox()
{
	// Create and attach a box component
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	// Set up overlap events
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AKillBox::OnOverlapBegin);
}

void AKillBox::BeginPlay()
{
	Super::BeginPlay();
}

void AKillBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKillBox, BoxComponent);
}

void AKillBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		ABlasterCharacter* Player = Cast<ABlasterCharacter>(OtherActor);
		if (Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("Kill box overlapped with %s"), *Player->GetName());
			Player->Elim();
		}
	}
	MulticastOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AKillBox::MulticastOnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		ABlasterCharacter* Player = Cast<ABlasterCharacter>(OtherActor);
		if (Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("Kill box overlapped with %s"), *Player->GetName());
			Player->Elim();
		}
	}
}

