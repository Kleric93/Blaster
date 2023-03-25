// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlagTypes.h"
#include "FlagStates.h"
#include "Blaster/BlasterTypes/Team.h"
#include "TeamsFlag.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlueFlagStateChanged, EFlagState, NewFlagStateBlue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRedFlagStateChanged, EFlagState, NewFlagStateRed);

UCLASS()
class BLASTER_API ATeamsFlag : public AActor
{
	GENERATED_BODY()
	
public:

	ATeamsFlag();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//void AttachToPlayerBackpack();
	void FlagBehavior();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFlagRespawn();

	void SetFlagState(EFlagState State);

	//UFUNCTION(Server, Reliable)
	void ServerDetachfromBackpack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDetachfromBackpack();

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnBlueFlagStateChanged OnBlueFlagStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnRedFlagStateChanged OnRedFlagStateChanged;

	FORCEINLINE EFlagState GetFlagState() const { return FlagState; }
	FORCEINLINE void SetFlagStateOD(EFlagState State) { FlagState = State; }
	FORCEINLINE UStaticMeshComponent* GetFlagMesh() const { return FlagMesh; }
	FORCEINLINE EFlagType GetFlagType() const { return FlagType; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE FVector GetInitialSpawnLocation() const { return InitialSpawnLocation; }

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
	class USoundBase* FlagReset;

	UPROPERTY(EditAnywhere)
	EFlagType FlagType;
	
	UPROPERTY(EditAnywhere)
	ETeam Team;

	UPROPERTY()
	FVector InitialSpawnLocation;

	FTimerHandle BindOverlapTimer;
	float BindOverlapTime = 0.25f;
	void BindOverlapTimerFinished();
};