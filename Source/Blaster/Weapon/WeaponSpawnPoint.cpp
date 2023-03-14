// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSpawnPoint.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"



AWeaponSpawnPoint::AWeaponSpawnPoint()
{
	PedestalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pedestal Mesh"));
	PedestalMesh->SetupAttachment(RootComponent);


	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	// Replicate the pedestal mesh's material
	PedestalMesh->SetIsReplicated(true);
	StartSpawnWeaponTimer(EWeaponState::EWS_Initial);

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

void AWeaponSpawnPoint::MulticastSetPedestalDefaultMaterial_Implementation()
{
	// Set the material on the server and replicate it to clients
	PedestalMesh->SetMaterial(0, DefaultPedestalMaterial);
}

void AWeaponSpawnPoint::MulticastSetPedestalOnMaterial_Implementation()
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

void AWeaponSpawnPoint::MulticastPlaySpawnSound_Implementation(USoundCue* Sound, FVector Location)
{
	// Play the sound on all clients
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location);
}

void AWeaponSpawnPoint::SpawnWeapon(EWeaponState State)
{
	int32 NumWeaponClasses = WeaponClasses.Num();
	if (NumWeaponClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumWeaponClasses - 1);

		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += 50.0f; // elevate the spawn location by 50 units

		FRotator SpawnRotation = GetActorRotation();
		FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);

		SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[Selection], SpawnTransform);
		EWeaponState WeaponState = SpawnedWeapon->GetWeaponState();

		if (HasAuthority() && WeaponState == EWeaponState::EWS_Initial)
		{
			// Start timer for spawning a new weapon
			//UE_LOG(LogTemp, Warning, TEXT("SpawnWeapon() called. Starting SpawnWeaponTimer..."));
			SpawnedWeapon->OnWeaponStateChanged.AddDynamic(this, &AWeaponSpawnPoint::StartSpawnWeaponTimer);
			//MulticastSetPedestalOnMaterial();
			//PedestalMesh->SetMaterial(0, WeaponSpawnedPedestalMaterial);
		}
	}
}



void AWeaponSpawnPoint::SpawnWeaponTimerFinished()
{
	if (HasAuthority())
	{
		SpawnWeapon(EWeaponState::EWS_Initial);
	}
	// Play the spawn sound on all clients using a multicast function
	MulticastPlaySpawnSound(SpawnSound, GetActorLocation());
	MulticastSetPedestalOnMaterial();
}

void AWeaponSpawnPoint::StartSpawnWeaponTimer(EWeaponState NewState)
{
	const float SpawnTime = FMath::FRandRange(SpawnWeaponTimeMin, SpawnWeaponTimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnWeaponTimer,
		this,
		&AWeaponSpawnPoint::SpawnWeaponTimerFinished,
		SpawnTime
	);

	PedestalMesh->SetMaterial(0, DefaultPedestalMaterial);
	MulticastSetPedestalDefaultMaterial();
}

void AWeaponSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpawnedWeapon && SpawnedWeapon->GetWeaponState() == EWeaponState::EWS_Initial)
	{
		SpawnedWeapon->GetWeaponMesh()->AddLocalRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}
}

