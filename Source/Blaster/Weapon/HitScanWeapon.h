// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Blaster/Blaster.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

	UFUNCTION(Server, Reliable)
		void ServerSpawnBulletHoles(const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnBulletHoles(const FHitResult& Hit);

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

private:

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* ImpactHolesMaterial;

	UPROPERTY(EditAnywhere)
	FVector DecalSize;

	UPROPERTY(EditAnywhere)
		float LifeSpan = 10.0f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
		USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

	///
	/// Trace end with scatter
	//
	UPROPERTY(EditAnywhere, Category = "WeaponScatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "WeaponScatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = "WeaponScatter")
	bool bUseScatter = false;
};
