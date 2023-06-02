// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ShieldPickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldBuffPickedUp);

/**
 * 
 */
UCLASS()
class BLASTER_API AShieldPickup : public APickup
{
	GENERATED_BODY()

public:

	FOnShieldBuffPickedUp OnShieldBuffPickedUp;

protected:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:

	UPROPERTY(EditAnywhere)
		float ShieldReplenishAmount = 100.f;

	UPROPERTY(EditAnywhere)
		float ShieldReplenishTime = 5.f;
	
};
