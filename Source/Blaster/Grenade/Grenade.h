// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeTypes.h"
#include "Grenade.generated.h"

UCLASS()
class BLASTER_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrenade();

protected:
	virtual void BeginPlay() override;

public:
	/** Getters */
	FORCEINLINE TObjectPtr<UAnimMontage> GetGrenadeMontage() { return ThrowMontage; }
	FORCEINLINE TObjectPtr<UAnimInstance> GetGrenadeInstance() { return GrenadeInstance; }

	FORCEINLINE EGrenadeType GetGrenadeType() { return GrenadeType; }

	/** Grenade Enum Setter */
	FORCEINLINE void SetGrenadeEnum(EGrenadeType NewGrenadeType) { GrenadeType = NewGrenadeType; }

public:
	void OnGrenadeThrow(FVector ForwardVector);

private:
	UFUNCTION()
		void TypeSwitch();

	UFUNCTION()
		void Explode();

	UFUNCTION()
		void Smoke();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<USkeletalMeshComponent> GrenadeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UAnimMontage> ThrowMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UNiagaraSystem> ExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum, meta = (AllowPrivateAccess = "true"))
		EGrenadeType GrenadeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage, meta = (AllowPrivateAccess = "true"))
		float DamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage, meta = (AllowPrivateAccess = "true"))
		float DamageAmount;

private:
	UPROPERTY()
		TObjectPtr<UAnimInstance> GrenadeInstance;

private:
	float ExplosionTimer;

	FTimerHandle ExplosionTimerHandle;
};


