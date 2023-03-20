// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/*
#include "CoreMinimal.h"
#include "Pickup.h"
#include "FlagTypes.h"
#include "Flag.generated.h"

UENUM(BlueprintType)
enum class EFlagState : uint8
{
	EFS_Initial UMETA(DisplayName = "Initial State"),
	EFS_Equipped UMETA(DisplayName = "Equipped"),
	EFS_Dropped UMETA(DisplayName = "Dropped"),

	EFS_MAX UMETA(DisplayName = "DefaultMAX"),

};


UCLASS()
class BLASTER_API AFlag : public APickup
{
	GENERATED_BODY()

public:

	AFlag();
	//void AttachToPlayerBackpack();
	void FlagBehavior(AActor* ActorInQuestion);
	void flagrespawn();
	void SetFlagState(EFlagState State);
	void DetachfromBackpack();
	FORCEINLINE EFlagState GetFlagState() const { return FlagState; }
	FORCEINLINE void SetFlagStateOD(EFlagState State) { FlagState = State; }
	FORCEINLINE UStaticMeshComponent* GetFlagMesh() const { return FlagMesh; }
protected:

	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadOnly, Category = "Flag")
	class ABlasterCharacter* OwningCharacter;

	UPROPERTY()
		class ABlasterPlayerController* OwningController;

private:
	UPROPERTY(VisibleAnywhere)
	EFlagState FlagState = EFlagState::EFS_Initial;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FlagMesh;

	UPROPERTY(EditAnywhere)
	EFlagType FlagType;

	UPROPERTY(EditAnywhere)
	FVector InitialSpawnLocation;
};
*/