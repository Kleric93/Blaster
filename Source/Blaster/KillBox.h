// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/KillZVolume.h"
#include "KillBox.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AKillBox : public AKillZVolume
{
	GENERATED_BODY()

public:
	AKillBox();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY(EditAnywhere)
		float ElimDelay = 1.f;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
		FVector BoxSize;

};
