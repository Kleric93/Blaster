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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class AWeapon>> WeaponClasses;

	UPROPERTY()
		AWeapon* SpawnedWeapon;

	void SpawnWeaponTimerFinished();

	void SpawnWeapon(EWeaponState NewState);


	UFUNCTION()
		void StartSpawnWeaponTimer(EWeaponState NewState);

private:
	FTimerHandle SpawnWeaponTimer;

	UPROPERTY(EditAnywhere)
		float SpawnWeaponTimeMin;

	UPROPERTY(EditAnywhere)
		float SpawnWeaponTimeMax;

	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;



public:	
	virtual void Tick(float DeltaTime) override;

};
