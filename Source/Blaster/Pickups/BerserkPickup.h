// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BerserkPickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBerserkBuffPickedUp, float, BuffTimer);


/**
 * 
 */
UCLASS()
class BLASTER_API ABerserkPickup : public APickup
{
	GENERATED_BODY()

public:

	FOnBerserkBuffPickedUp OnBerserkBuffPickedUp;

private:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);


	UPROPERTY(EditAnywhere)
	float BerserkBuffTime = 15.f;

	UPROPERTY()
	class ABlasterCharacter* Character;
};
