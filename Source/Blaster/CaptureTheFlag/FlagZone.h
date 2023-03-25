// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blaster/BlasterTypes/Team.h"
#include "FlagZone.generated.h"

UCLASS()
class BLASTER_API AFlagZone : public AActor
{
	GENERATED_BODY()

public:
	AFlagZone();

	UPROPERTY(EditAnywhere)
	ETeam Team;

	UPROPERTY()
	class ATeamsFlag* Flag;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintPure, Category = "Flag")
		bool IsFlagInBase(ETeam ScoringTeam) const;

private:

	UPROPERTY(EditAnywhere)
	class USphereComponent* ZoneSphere;

	UPROPERTY(EditAnywhere)
	class USoundBase* CaptureSound;

};