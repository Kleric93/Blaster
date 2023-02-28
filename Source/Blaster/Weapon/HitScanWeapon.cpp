// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalmeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Blaster/Blaster.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "WeaponTypes.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
/*
void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if (BlasterCharacter && HasAuthority() && &InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				BlasterCharacter,
				BaseDamage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
			ServerSpawnBulletHoles(FireHit);
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				FireHit.ImpactPoint
			);
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}*/

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		float Distance = (FireHit.ImpactPoint - Start).Size() / 100.f;
		float DamageMultiplier = 1.f;
		if (Distance > FullDamageDistance && Distance <= LeastDamageDistance)
		{
			DamageMultiplier = FMath::Lerp(1.f, 0.1f, (Distance - FullDamageDistance) /  LeastDamageDistance);
		}
		else if (Distance > FullDamageDistance)
		{
			DamageMultiplier = 0.1f;
		}

		float FinalDamage = Damage * DamageMultiplier;

		UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
		//UE_LOG(LogTemp, Warning, TEXT("ActorHit: %s"), *FireHit.GetActor()->GetName());


		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if (BlasterCharacter && HasAuthority() && &InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				BlasterCharacter,
				FinalDamage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
			ServerSpawnBulletHoles(FireHit);
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				FireHit.ImpactPoint
			);
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		FCollisionQueryParams TraceParams(FName(TEXT("FireTrace")), true, OwnerPawn);
		TraceParams.AddIgnoredActor(this);

		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility,
			TraceParams
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void AHitScanWeapon::ServerSpawnBulletHoles_Implementation(const FHitResult& Hit)
{
	MulticastSpawnBulletHoles(Hit);
}

void AHitScanWeapon::MulticastSpawnBulletHoles_Implementation(const FHitResult& Hit)
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
		//DecalSize = FVector(16.0f, 16.0f, 16.0f); // Adjust this as needed
		UDecalComponent* DecalBullets = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, DecalSize, HitLocation, Rotation, LifeSpan);

		
		// Set the LOD properties of the DecalActor component
		if (DecalBullets)
		{
			DecalBullets->SetFadeScreenSize(0.001f); // Set the screen size at which the decal will begin to fade out
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	UCombatComponent* CombatComponent = GetOwner()->FindComponentByClass<UCombatComponent>();

	if (CombatComponent && CombatComponent->IsAiming())
	{
		FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadiusWhenAimed);
		FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		return FVector(TraceStart + ToEndLoc * TRACE_LENGHT / ToEndLoc.Size());
	}
	else
	{
		FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		return FVector(TraceStart + ToEndLoc * TRACE_LENGHT / ToEndLoc.Size());
	}

	/*
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGHT / ToEndLoc.Size()),
		FColor::Cyan,
		true
	);*/

}