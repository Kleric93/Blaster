// Fill out your copyright notice in the Description page of Project Settings.
/*

#include "Flag.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterTypes/Team.h"
#include "FlagTypes.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

AFlag::AFlag()
{
	FlagMesh = CreateDefaultSubobject < UStaticMeshComponent>(TEXT("Flagmesh"));
	FlagMesh->SetRelativeScale3D(FVector(.4f, .4f, .4f));
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetRootComponent(FlagMesh);

	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlag::OnSphereOverlap);
}

void AFlag::BeginPlay()
{
}

void AFlag::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter && BlasterCharacter->GetPlayerState<ABlasterPlayerState>())
	{
		FlagBehavior(BlasterCharacter);
		BlasterCharacter->GetCombat()->EquipFlag(this);
		UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
	}
}

void AFlag::DetachfromBackpack()
{
	if (FlagState == EFlagState::EFS_Equipped)
	{
		SetFlagState(EFlagState::EFS_Dropped);
		UE_LOG(LogTemp, Error, TEXT("Flag is dropped from FLAG.CPP"));

		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		FlagMesh->DetachFromComponent(DetachRules);

		SetOwner(nullptr);
		OwningCharacter = nullptr;
		OwningController = nullptr;
	}
}
/*
void AFlag::AttachToPlayerBackpack()
{
	OwningController = OwningCharacter->GetController<ABlasterPlayerController>();
	OwningCharacter->GetCombat()->AttachFlagToBackpack(this);
	SetOwner(OwningCharacter);

	FlagState = EFlagState::EFS_Equipped;
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
}*/


/*
void AFlag::FlagBehavior(AActor* ActorInQuestion)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ActorInQuestion);
	if (OwningCharacter == nullptr || OwningCharacter->GetCombat() == nullptr) return;
	ABlasterPlayerState* BlasterPlayerState = OwningCharacter->GetPlayerState<ABlasterPlayerState>();
	BlasterPlayerState = BlasterPlayerState == nullptr ? Cast<ABlasterPlayerState>(OwningCharacter->GetPlayerState()) : BlasterPlayerState;

	if (BlasterPlayerState->GetTeam() == ETeam::ET_BlueTeam)
	{
		switch (FlagType)
		{
		case EFlagType::EFT_RedFlag:
			BlasterCharacter->GetCombat()->EquipFlag(this);
			break;
		case EFlagType::EFT_BlueFlag:
			flagrespawn();
			break;
		default:
			break;
		}

	}
	else if (BlasterPlayerState->GetTeam() == ETeam::ET_RedTeam)
	{
		switch (FlagType)
		{
		case EFlagType::EFT_RedFlag:
			flagrespawn();
			break;
		case EFlagType::EFT_BlueFlag:
			BlasterCharacter->GetCombat()->EquipFlag(this);
			break;
		default:
			break;
		}
	}

}

void AFlag::flagrespawn()
{
	SetActorLocation(InitialSpawnLocation);
	FlagState = EFlagState::EFS_Initial;
}

void AFlag::SetFlagState(EFlagState State)
{
	switch (FlagState)
	{
	case EFlagState::EFS_Initial:
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		FlagMesh->SetSimulatePhysics(false);
		FlagMesh->SetEnableGravity(false);
		break;
	case EFlagState::EFS_Equipped:
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetSimulatePhysics(false);
		FlagMesh->SetEnableGravity(false);
		break;
	case EFlagState::EFS_Dropped:
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		FlagMesh->SetSimulatePhysics(true);
		FlagMesh->SetEnableGravity(true);
		break;
	case EFlagState::EFS_MAX:
		break;
	default:
		break;
	}
}

*/