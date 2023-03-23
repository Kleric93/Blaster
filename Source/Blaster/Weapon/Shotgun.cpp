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
		TMap<ABlasterCharacter*, uint32> HeadshotHitMap;

		FHitResult FireHit;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadShot)
				{
					if (HeadshotHitMap.Contains(BlasterCharacter)) HeadshotHitMap[BlasterCharacter]++;
					else HeadshotHitMap.Emplace(BlasterCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
					else HitMap.Emplace(BlasterCharacter, 1);
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
				if (HasAuthority())
				{
					ServerSpawnBulletHoles(FireHit);
				}
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

		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		float Distance = (FireHit.ImpactPoint - Start).Size() / 100.f;
		float DamageMultiplier;

		if (Distance <= FullDamageDistance)
		{
			DamageMultiplier = 1.f;
		}
		else if (Distance > FullDamageDistance && Distance <= LeastDamageDistance)
		{
			DamageMultiplier = FMath::Lerp(1.f, 0.1f, (Distance - FullDamageDistance) / LeastDamageDistance);
		}
		else if (Distance > LeastDamageDistance)
		{
			DamageMultiplier = 0.1f;
		}

		TArray<ABlasterCharacter*> HitCharacters;

		//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
		// maps character hit to total damage
		TMap<ABlasterCharacter*, float> DamageMap;

		// calculate bodyshot damage by multiplying times hit x damage - store in damagemap
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && InstigatorController)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);


				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// calculate headshot damage by multiplying times Hit x HeadshotDamage - store in DamageMap
		for (auto HeadshotHitPair : HeadshotHitMap)
		{
			if (HeadshotHitPair.Key)
			{
				if (DamageMap.Contains(HeadshotHitPair.Key)) DamageMap[HeadshotHitPair.Key] += HeadshotHitPair.Value * HeadshotDamage;
				else DamageMap.Emplace(HeadshotHitPair.Key, HeadshotHitPair.Value * HeadshotDamage);

				HitCharacters.AddUnique(HeadshotHitPair.Key);
			}
		}
		// Loop throgh damage map to get total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					float FinalDamage = DamagePair.Value * DamageMultiplier;

					UGameplayStatics::ApplyDamage(
						DamagePair.Key, // char that was hit
						FinalDamage, // damage calculated in the two for loops above // TODO, CHECK IF * DamageMultiplier is a valid argument in the function (should work, required playtesting)
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
					//UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
					//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
					//DrawDebugSphere(GetWorld(), FireHit.ImpactPoint, 100.f, 30, FColor::Orange, false, 20.f);

					// Draw debug sphere for HitLocation
					//DrawDebugSphere(GetWorld(), Start, 10.f, 16, FColor::Blue, false, 20.f);
				}
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


	/*
	bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
	if (HasAuthority() && bCauseAuthDamage)
	{
		UGameplayStatics::ApplyDamage(
			BlasterCharacter, // char that was hit
			FinalDamage * HitPair.Value,
			InstigatorController,
			this,
			UDamageType::StaticClass()
		);
		//UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
		//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
	}*/
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
