// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class BLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPedestalDefaultMaterial();

UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPedestalOnMaterial();

UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySpawnSound(USoundCue* Sound, FVector Location);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	APickup* SpawnedPickup;

	void SpawnPickup();
	void SpawnPickupTimerFinished();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* DefaultPedestalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* WeaponSpawnedPedestalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraComponent* PedestalParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraSystem* NiagaraSystemParticles;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* PedestalMesh;

	UPROPERTY(EditAnywhere)
		class USoundCue* SpawnSound;
private:
	FTimerHandle SpawnPickupTimer;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax;

public:	
	virtual void Tick(float DeltaTime) override;

};
