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

	void ServerSpawnBulletHoles(const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnBulletHoles(const FHitResult& Hit);

protected:

	virtual void BeginPlay() override;

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);
	void SpawnSparksSystem();

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere)
	USoundCue* PlayerHitSound;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ImpactHolesMaterial;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ChargerSparksSystem;

	UPROPERTY()
	class UNiagaraComponent* ChargerSparksSystemComponent;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	FVector DecalSize;

	UPROPERTY(EditAnywhere)
		float LifeSpan = 10.0f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
		USoundCue* FireSound;

};
