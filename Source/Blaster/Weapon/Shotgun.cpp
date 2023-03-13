// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalmeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"


/*void AShotgun::Fire(const FVector& HitTarget)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		AWeapon::Fire(HitTarget);
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		TMap<ABlasterCharacter*, uint32> HitMap;
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && HasAuthority() && &InstigatorController)
			{
				if (HitMap.Contains(BlasterCharacter))
				{
					HitMap[BlasterCharacter]++;
				}
				else
				{
					HitMap.Emplace(BlasterCharacter, 1);
				}
				// Calculate damage based on distance and update the hit result
				float Distance = (FireHit.ImpactPoint - Start).Size() / 100.f;
				float DamageMultiplier = 1.f;
				if (Distance > FullDamageDistance && Distance <= LeastDamageDistance)
				{
					DamageMultiplier = FMath::Lerp(1.f, 0.1f, (Distance - FullDamageDistance) / LeastDamageDistance);
				}
				else if (Distance > FullDamageDistance)
				{
					DamageMultiplier = 0.1f;
				}

				float FinalDamage = Damage * DamageMultiplier;

				UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
				UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);

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
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}
		for (auto HitPair : HitMap)
		{
			if (InstigatorController)
			{
				// Do nothing here since damage was already applied in the previous loop
			}
		}
	}
}*/

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		
		// maps hit character to number of times hit
		TMap<ABlasterCharacter*, uint32> HitMap;

		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				if (HitMap.Contains(BlasterCharacter))
				{
					HitMap[BlasterCharacter]++;
				}
				else
				{
					HitMap.Emplace(BlasterCharacter, 1);
				}
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
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}

		// Calculate damage based on distance and update the hit result

		FHitResult FireHit;
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		float Distance = (FireHit.ImpactPoint - Start).Size() / 100.f;
		float DamageMultiplier = 1.f;
		if (Distance > FullDamageDistance && Distance <= LeastDamageDistance)
		{
			DamageMultiplier = FMath::Lerp(1.f, 0.1f, (Distance - FullDamageDistance) / LeastDamageDistance);
		}
		else if (Distance > FullDamageDistance)
		{
			DamageMultiplier = 0.1f;
		}
		float FinalDamage = Damage * DamageMultiplier;

		TArray<ABlasterCharacter*> HitCharacters;

		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && InstigatorController)
			{
				if (HasAuthority() && !bUseServerSideRewind)
				{
					UGameplayStatics::ApplyDamage(
						BlasterCharacter, // char that was hit
						FinalDamage,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
					UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
					UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
				}
				HitCharacters.Add(HitPair.Key);
			}
		}
		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
			if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetlagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetlagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;

	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector TraceStart = SocketTransform.GetLocation();

	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	UCombatComponent* CombatComponent = GetOwner()->FindComponentByClass<UCombatComponent>();

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		if (CombatComponent && CombatComponent->IsAiming())
		{
			FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadiusWhenAimed);
			FVector EndLoc = SphereCenter + RandVec;
			FVector ToEndLoc = EndLoc - TraceStart;
			ToEndLoc = (TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());

			HitTargets.Add(ToEndLoc);
		}
		else
		{
			FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
			FVector EndLoc = SphereCenter + RandVec;
			FVector ToEndLoc = EndLoc - TraceStart;
			ToEndLoc = (TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());

			HitTargets.Add(ToEndLoc);
		}
	}

	/*
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Cyan,
		true
	);*/
}
