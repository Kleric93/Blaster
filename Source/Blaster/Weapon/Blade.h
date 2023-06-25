// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Blade.generated.h"

class UMaterialInterface;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlade : public AWeapon
{
	GENERATED_BODY()

public:

	ABlade();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetBladeMaterialOff();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetBladeMaterialOn();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetBladeParticlesOn();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetBladeParticlesOff();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastBladeDamage(const FString& PlayerName, int32 HitscanDamage);

protected:


	virtual	void Tick(float DeltaTime) override;

	virtual void Fire(const FVector& HitTarget) override;
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BladeMats)
		UMaterialInterface* BladeMaterialOff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BladeMats)
		UMaterialInterface* BladeMaterialOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraComponent* BladeSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraSystem* BladeSystemParticles;


private:

	UPROPERTY()
		FVector TargetLocation;
	UPROPERTY()
		bool bIsTeleporting;

	UPROPERTY()
	FVector NewPosition; // Add this property

};
