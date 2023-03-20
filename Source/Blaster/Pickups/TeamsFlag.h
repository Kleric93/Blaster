// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlagTypes.h"
#include "Blaster/BlasterTypes/Team.h"
#include "TeamsFlag.generated.h"

UENUM(BlueprintType)
enum class EFlagState : uint8
{
	EFS_Initial UMETA(DisplayName = "Initial State"),
	EFS_Equipped UMETA(DisplayName = "Equipped"),
	EFS_Dropped UMETA(DisplayName = "Dropped"),

	EFS_MAX UMETA(DisplayName = "DefaultMAX"),

};

UCLASS()
class BLASTER_API ATeamsFlag : public AActor
{
	GENERATED_BODY()
	
public:

	ATeamsFlag();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//void AttachToPlayerBackpack();
	void FlagBehavior();
	void flagrespawn();
	void SetFlagState(EFlagState State);

	UFUNCTION(Server, Reliable)
	void ServerDetachfromBackpack();


	UFUNCTION(NetMulticast, Reliable)
	void MulticastDetachfromBackpack();

	FORCEINLINE EFlagState GetFlagState() const { return FlagState; }
	FORCEINLINE void SetFlagStateOD(EFlagState State) { FlagState = State; }
	FORCEINLINE UStaticMeshComponent* GetFlagMesh() const { return FlagMesh; }
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

	UPROPERTY(BlueprintReadOnly, Category = "Flag")
		class ABlasterCharacter* OwningCharacter;

	UPROPERTY()
		class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY()
	class ABlasterPlayerController* OwningController;

	UPROPERTY(VisibleAnywhere, Category = "Flagproperties")
	class USphereComponent* OverlapSphere;

private:

	UPROPERTY(ReplicatedUsing = OnRep_FlagState, VisibleAnywhere)
	EFlagState FlagState;

	UFUNCTION()
	void OnRep_FlagState();

	UPROPERTY(VisibleAnywhere, Category = "Flagproperties")
	class UStaticMeshComponent* FlagMesh;

	UPROPERTY(EditAnywhere)
	EFlagType FlagType;
	
	UPROPERTY(EditAnywhere)
	ETeam Team;

	UPROPERTY(EditAnywhere)
	FVector InitialSpawnLocation;

	FTimerHandle BindOverlapTimer;
	float BindOverlapTime = 0.25f;
	void BindOverlapTimerFinished();
};
