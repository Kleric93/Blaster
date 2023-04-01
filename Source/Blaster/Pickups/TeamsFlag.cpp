// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsFlag.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterTypes/Team.h"
#include "FlagTypes.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATeamsFlag::ATeamsFlag()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flagmesh"));
	FlagMesh->SetupAttachment(RootComponent);
	SetRootComponent(FlagMesh);
	FlagMesh->SetRelativeScale3D(FVector(.4f, .4f, .4f));

	FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(FlagMesh);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ATeamsFlag::OnSphereOverlap);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(FlagMesh);
}

// Called when the game starts or when spawned
void ATeamsFlag::BeginPlay()
{
	Super::BeginPlay();

	InitialSpawnLocation = GetActorLocation();

	FlagState = EFlagState::EFS_Initial;

	if (this->ActorHasTag("BlueFlagTag"))
	{
		UE_LOG(LogTemp, Error, TEXT("Blue delegate being called IN BEGINPLAY"));

		OnBlueFlagStateChanged.Broadcast(EFlagState::EFS_Initial);
	}

	if (this->ActorHasTag("RedFlagTag"))
	{
		//UE_LOG(LogTemp, Error, TEXT("Red delegate being called IN Detach DROPPED"));

		OnRedFlagStateChanged.Broadcast(EFlagState::EFS_Initial);
	}

	FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			BindOverlapTimer,
			this,
			&ATeamsFlag::BindOverlapTimerFinished,
			BindOverlapTime
		);
	}
}

void ATeamsFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CapsuleComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void ATeamsFlag::BindOverlapTimerFinished()
{
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ATeamsFlag::OnSphereOverlap);
}

void ATeamsFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamsFlag, FlagState);
}

void ATeamsFlag::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (FlagState == EFlagState::EFS_Equipped) return;
	if (BlasterCharacter)
	{

		if (FlagType == EFlagType::EFT_RedFlag && BlasterCharacter->GetTeam() == ETeam::ET_RedTeam) 
		{
			if (this->ActorHasTag("RedFlagTag"))
			{
				//UE_LOG(LogTemp, Error, TEXT("Red delegate being called IN Detach DROPPED"));

				OnRedFlagStateChanged.Broadcast(EFlagState::EFS_Initial);
			}
			MulticastFlagRespawn();
		}
		else if (FlagType == EFlagType::EFT_BlueFlag && BlasterCharacter->GetTeam() == ETeam::ET_BlueTeam)
		{
			if (this->ActorHasTag("BlueFlagTag"))
			{
				UE_LOG(LogTemp, Error, TEXT("Blue delegate being called IN OverlapSphere Initial state"));

				OnBlueFlagStateChanged.Broadcast(EFlagState::EFS_Initial);
			}
			MulticastFlagRespawn();
		}
		else if (FlagType == EFlagType::EFT_RedFlag && BlasterCharacter->GetTeam() == ETeam::ET_BlueTeam)
		{
			if (this->ActorHasTag("RedFlagTag"))
			{
				//UE_LOG(LogTemp, Error, TEXT("Red delegate being called IN Detach DROPPED"));

				OnRedFlagStateChanged.Broadcast(EFlagState::EFS_Equipped);
			}
			BlasterCharacter->GetCombat()->EquipFlag(this);
			//UE_LOG(LogTemp, Error, TEXT("Blue team character overlapped with red flag"));
		}
		else if (FlagType == EFlagType::EFT_BlueFlag && BlasterCharacter->GetTeam() == ETeam::ET_RedTeam)
		{
			if (this->ActorHasTag("BlueFlagTag"))
			{
				UE_LOG(LogTemp, Error, TEXT("Blue delegate being called IN OverlapSphere Equipped State"));

				OnBlueFlagStateChanged.Broadcast(EFlagState::EFS_Equipped);
			}
			BlasterCharacter->GetCombat()->EquipFlag(this);
			//UE_LOG(LogTemp, Error, TEXT("Blue team character overlapped with red flag"));
		}
		//FlagBehavior();
		UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BLASTER WAS NULL IN FLAG.CPP"));
	}
}

void ATeamsFlag::ServerDetachfromBackpack()
{
	MulticastDetachfromBackpack();

	if (FlagState == EFlagState::EFS_Equipped)
	{
		UE_LOG(LogTemp, Error, TEXT("Flag is dropped from FLAG.CPP"));

		FlagState = EFlagState::EFS_Dropped;

		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		FlagMesh->DetachFromComponent(DetachRules);

		// Set the rotation to 0, 0, 0
		FRotator NewRotation = FRotator::ZeroRotator;
		FlagMesh->SetWorldRotation(NewRotation);

		SetOwner(nullptr);
		OwningCharacter = nullptr;
		OwningController = nullptr;


	}
}

void ATeamsFlag::MulticastDetachfromBackpack_Implementation()
{
	if (this->ActorHasTag("BlueFlagTag"))
	{
		//UE_LOG(LogTemp, Error, TEXT("Blue delegate being called IN Detach DROPPED"));

		OnBlueFlagStateChanged.Broadcast(EFlagState::EFS_Dropped);
	}
	if (this->ActorHasTag("RedFlagTag"))
	{
		//UE_LOG(LogTemp, Error, TEXT("Red delegate being called IN Detach DROPPED"));

		OnRedFlagStateChanged.Broadcast(EFlagState::EFS_Dropped);
	}
	if (FlagState == EFlagState::EFS_Equipped)
	{
		UE_LOG(LogTemp, Error, TEXT("Flag is dropped from FLAG.CPP"));

		FlagState = EFlagState::EFS_Dropped;


		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		FlagMesh->DetachFromComponent(DetachRules);

		// Set the rotation to 0, 0, 0
		FRotator NewRotation = FRotator::ZeroRotator;
		FlagMesh->SetWorldRotation(NewRotation);

		SetOwner(nullptr);
		OwningCharacter = nullptr;
		OwningController = nullptr;
	}
}
/*
void ATeamsFlag::FlagBehavior()
{
	if (OwningCharacter == nullptr || OwningCharacter->GetCombat() == nullptr) return;
	BlasterPlayerState = BlasterPlayerState == nullptr ? Cast<ABlasterPlayerState>(OwningCharacter->GetPlayerState()) : BlasterPlayerState;

	if (BlasterPlayerState->GetTeam() == ETeam::ET_BlueTeam)
	{
		switch (FlagType)
		{
		case EFlagType::EFT_RedFlag:
			OwningCharacter->GetCombat()->EquipFlag(this);
			UE_LOG(LogTemp, Error, TEXT("Blue team character overlapped with red flag"));
			break;
		case EFlagType::EFT_BlueFlag:
			UE_LOG(LogTemp, Error, TEXT("Blue team character overlapped with blue flag"));
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
			UE_LOG(LogTemp, Error, TEXT("red team character overlapped with red flag"));

			flagrespawn();
			break;
		case EFlagType::EFT_BlueFlag:
			OwningCharacter->GetCombat()->EquipFlag(this);
			UE_LOG(LogTemp, Error, TEXT("red team character overlapped with Blue flag"));

			break;
		default:
			break;
		}

	}

}*/

void ATeamsFlag::MulticastFlagRespawn_Implementation()
{
	if (GetActorLocation() == InitialSpawnLocation) return;
	SetActorLocation(InitialSpawnLocation);
	FlagState = EFlagState::EFS_Initial;
	UGameplayStatics::PlaySound2D(
		GetWorld(),
		FlagReset
	);
}

void ATeamsFlag::SetFlagState(EFlagState State)
{

	FlagState = State;
	switch (FlagState)
	{
	case EFlagState::EFS_Initial:
		FlagMesh->SetSimulatePhysics(false);
		FlagMesh->SetEnableGravity(false);
		FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		break;
	case EFlagState::EFS_Equipped:
		FlagMesh->SetSimulatePhysics(false);
		FlagMesh->SetEnableGravity(false);

		FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		break;
	case EFlagState::EFS_Dropped:
		if (HasAuthority())
		{
			OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		FlagMesh->SetSimulatePhysics(true);
		FlagMesh->SetEnableGravity(true);
		FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		break;
	case EFlagState::EFS_MAX:
		break;
	default:
		break;
	}
}

void ATeamsFlag::OnRep_FlagState()
{
	switch (FlagState)
	{
	case EFlagState::EFS_Initial:
		FlagMesh->SetSimulatePhysics(false);
		FlagMesh->SetEnableGravity(false);
		FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		break;
	case EFlagState::EFS_Equipped:
		FlagMesh->SetSimulatePhysics(false);
		FlagMesh->SetEnableGravity(false);

		FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		break;
	case EFlagState::EFS_Dropped:
		if (HasAuthority())
		{
			OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		FlagMesh->SetSimulatePhysics(true);
		FlagMesh->SetEnableGravity(true);
		FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);
		FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


		break;
	case EFlagState::EFS_MAX:
		break;
	default:
		break;
	}
}