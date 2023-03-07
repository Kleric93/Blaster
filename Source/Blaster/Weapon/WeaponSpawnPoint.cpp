// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSpawnPoint.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"




AWeaponSpawnPoint::AWeaponSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnWeaponTimer(EWeaponState::EWS_Initial);
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
			UE_LOG(LogTemp, Warning, TEXT("SpawnWeapon() called. Starting SpawnWeaponTimer..."));
			SpawnedWeapon->OnWeaponStateChanged.AddDynamic(this, &AWeaponSpawnPoint::StartSpawnWeaponTimer);
		}
	}
}



void AWeaponSpawnPoint::SpawnWeaponTimerFinished()
{
	if (HasAuthority())
	{
		SpawnWeapon(EWeaponState::EWS_Initial);
	}
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
}

void AWeaponSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpawnedWeapon && SpawnedWeapon->GetWeaponState() == EWeaponState::EWS_Initial)
	{
		SpawnedWeapon->GetWeaponMesh()->AddLocalRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}
}

