// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlagRespawner.generated.h"

UCLASS()
class BLASTER_API AFlagRespawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlagRespawner();

	UFUNCTION()
		virtual void OnComponentBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* RespawnBox;

public:	
	virtual void Tick(float DeltaTime) override;

};
