// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawnPoint.h"
#include "Pickup.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "SpeedPickup.h"
#include "JumpPickup.h"
#include "BerserkPickup.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	PedestalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pedestal Mesh"));
	PedestalMesh->SetupAttachment(RootComponent);
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnPickupTimer((AActor*)nullptr);

	PedestalParticlesComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystemParticles, GetActorLocation());
	if (PedestalParticlesComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Particles DEactivated BeginPlay"));

		PedestalParticlesComponent->Deactivate();
	}

	// Check if the pedestal mesh is currently using the default material
	if (PedestalMesh->GetMaterial(0) == DefaultPedestalMaterial)
	{
		// If it is, swap it to the special material
		PedestalMesh->SetMaterial(0, DefaultPedestalMaterial);
		MulticastSetPedestalDefaultMaterial();
	}
}

void APickupSpawnPoint::MulticastSetPedestalDefaultMaterial_Implementation()
{
	// Set the material on the server and replicate it to clients
	PedestalMesh->SetMaterial(0, DefaultPedestalMaterial);
}

void APickupSpawnPoint::MulticastSetPedestalOnMaterial_Implementation()
{
	// Set the material on the clients as well
	PedestalMesh->SetMaterial(0, WeaponSpawnedPedestalMaterial);

	PedestalParticlesComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystemParticles, GetActorLocation());
	if (PedestalParticlesComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Particles activated in SpawnWeaponTimerFInished"));
		PedestalParticlesComponent->Activate();
	}
}

void APickupSpawnPoint::MulticastPlaySpawnSound_Implementation(USoundCue* Sound, FVector Location)
{
	// Play the sound on all clients
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location);
}

void APickupSpawnPoint::SpawnPickup()
{
	int32 NumPickupClasses = PickupClasses.Num();
	if (NumPickupClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumPickupClasses - 1);
		SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[Selection], GetActorTransform());

		if (HasAuthority() && SpawnedPickup)
		{
			SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::StartSpawnPickupTimer);
		}
		MulticastSpawnPickupDelegate();
	}
}

void APickupSpawnPoint::MulticastSpawnPickupDelegate_Implementation()
{
	ASpeedPickup* SpeedPickup = Cast<ASpeedPickup>(SpawnedPickup);
	AJumpPickup* JumpPickup = Cast<AJumpPickup>(SpawnedPickup);
	ABerserkPickup* BerserkPickup = Cast<ABerserkPickup>(SpawnedPickup);

	if (SpeedPickup)
	{
		OnSpeedBuffSpawned.Broadcast(this);
	}
	if (JumpPickup)
	{
		OnJumpBuffSpawned.Broadcast(this);
	}
	if (BerserkPickup)
	{
		OnBerserkBuffSpawned.Broadcast(this);
	}
}

void APickupSpawnPoint::SpawnPickupTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
	MulticastPlaySpawnSound(SpawnSound, GetActorLocation());
	MulticastSetPedestalOnMaterial();
}

void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnPickupTimeMin, SpawnPickupTimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickupTimer,
		this,
		&APickupSpawnPoint::SpawnPickupTimerFinished,
		SpawnTime
	);
	MulticastSetPedestalDefaultMaterial();
}

void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

