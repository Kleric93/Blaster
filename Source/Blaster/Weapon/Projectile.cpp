// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ServerSpawnBulletHoles(Hit);

	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
}

void AProjectile::ExplodeDamage()
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, // world
				Damage, //base dmg
				30.f, //min. dmg
				GetActorLocation(), // origin of radial dmg
				DamageInnerRadius, // damage inner radius
				DamageOuterRadius, // damage outer radius
				1.f, // damage falloff
				UDamageType::StaticClass(), // damage type class
				TArray<AActor*>(), // ignored actors
				this, // dmg causer
				FiringController // instigator controller
			);
		}
	}
}


void AProjectile::ServerSpawnBulletHoles_Implementation(const FHitResult& Hit)
{
	MulticastSpawnBulletHoles(Hit);
}

bool AProjectile::ServerSpawnBulletHoles_Validate(const FHitResult& Hit)
{
	// Check that the hit data is not null
	if (Hit.bBlockingHit == false)
	{
		//UE_LOG(LogTemp, Error, TEXT("ServerSpawnBulletHoles_Validate: Invalid Hit data"));
		return false;
	}

	// Check that the hit location is within a reasonable range from the projectile's current location
	if (FVector::DistSquared(Hit.Location, GetActorLocation()) > 1000000.0f) // 1000 units squared
	{
		//UE_LOG(LogTemp, Error, TEXT("ServerSpawnBulletHoles_Validate: Hit location out of range"));
		return false;
	}

	// Check that the ImpactHoles material is not null
	if (ImpactHolesMaterial == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("ServerSpawnBulletHoles_Validate: ImpactHoles material is null"));
		return false;
	}

	// Check that the player who fired the projectile has the authority to spawn bullet holes on the server
	if (GetOwner()->HasAuthority() == false)
	{
		//UE_LOG(LogTemp, Error, TEXT("ServerSpawnBulletHoles_Validate: Owner does not have authority"));
		return false;
	}

	// All validation checks passed
	return true;
}


void AProjectile::MulticastSpawnBulletHoles_Implementation(const FHitResult& Hit)
{
	if (GetWorld())
	{
		// Set up the decal parameters
		FVector HitLocation = Hit.ImpactPoint;
		FVector HitNormal = Hit.ImpactNormal;
		FRotator Rotation = HitNormal.Rotation();

		float RandomRollOffset = FMath::RandRange(-180.0f, 180.0f);
		Rotation = FRotator(Rotation.Pitch, Rotation.Yaw, Rotation.Roll + RandomRollOffset);

		// Spawn the decal
		UMaterialInterface* DecalMaterial = ImpactHolesMaterial;
		FVector DecalSize = FVector(16.0f, 16.0f, 16.0f); // Adjust this as needed
		UDecalComponent* DecalBullets = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, DecalSize, HitLocation, Rotation, LifeSpan);

		if (DecalBullets)
		{
			DecalBullets->SetFadeScreenSize(0.001f); // Set the screen size at which the decal will begin to fade out
		}
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectile::DestroyTimerFinished,
		DestroyTime
	);
}

void AProjectile::DestroyTimerFinished()
{
	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

