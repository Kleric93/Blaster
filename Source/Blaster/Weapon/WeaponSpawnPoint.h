// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawnPoint.generated.h"


UCLASS()
class BLASTER_API AWeaponSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponSpawnPoint();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetPedestalDefaultMaterial();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetPedestalOnMaterial();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlaySpawnSound(USoundCue* Sound, FVector Location);

	UFUNCTION()
		void StartSpawnWeaponTimer(EWeaponState NewState);

	UFUNCTION()
		void SpawnWeaponTimerFinished();

	UFUNCTION()
		void SpawnWeapon(EWeaponState State);

	FORCEINLINE bool IsWeaponSpawned() const { return SpawnedWeapon != nullptr; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class AWeapon>> WeaponClasses;

	UPROPERTY()
		AWeapon* SpawnedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* DefaultPedestalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* WeaponSpawnedPedestalMaterial;

	UPROPERTY(EditAnywhere)
	class USoundCue* SpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraComponent* PedestalParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraSystem* NiagaraSystemParticles;

private:
	FTimerHandle SpawnWeaponTimer;

	UPROPERTY(EditAnywhere)
		float SpawnWeaponTimeMin;

	UPROPERTY(EditAnywhere)
		float SpawnWeaponTimeMax;

	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PedestalMesh;

public:	
	virtual void Tick(float DeltaTime) override;

};
