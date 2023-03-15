// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnBulletHoles(const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnBulletHoles(const FHitResult& Hit);

	//
	/// Used with SSR
	//

	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	// Only Set this for grenades and rockets
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	// doesn't matter for Grenades and Rockets
	UPROPERTY(EditAnywhere)
	float HeadshotDamage = 40.f;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 20000;

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* TrailSystem;

	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;



	UPROPERTY(EditAnywhere)
	UMaterialInterface* ImpactHolesMaterial;

	UPROPERTY(EditAnywhere)
	float LifeSpan = 10.0f;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 10.f;

public:	
};
