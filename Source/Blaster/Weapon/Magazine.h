// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magazine.generated.h"

UCLASS()
class BLASTER_API AMagazine : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagazine();

protected:
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* MagazineMesh;

	UPROPERTY(EditAnywhere)
		float MagazineEjectionImpulse;

	UPROPERTY(EditAnywhere)
		class USoundCue* MagazineSound;

	bool bSoundPlayed;
	bool bHasHit;

public:	

};
