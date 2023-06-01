// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

class ABlasterCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAISpawned, AAISpawner*, AISpawner);


UCLASS()
class BLASTER_API AAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAISpawner();

	FOnAISpawned OnAISpawned;

	UFUNCTION()
		void SetPedestalDefaultMaterial();

	UFUNCTION()
		void SetPedestalOnMaterial();

	UFUNCTION()
		void PlaySpawnSound(USoundCue* Sound, FVector Location);

	void SpawnAI();

	UPROPERTY()
		ABlasterCharacter* SpawnedAI;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABlasterCharacter> CharacterClass;

	UFUNCTION()
		void SpawnAIDelegate();

	UFUNCTION()
	void SpawnAITimerFinished();

	UFUNCTION()
		void StartSpawnAITimer(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* DefaultPedestalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
		UMaterialInterface* WeaponSpawnedPedestalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraComponent* PedestalParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UNiagaraSystem* NiagaraSystemParticles;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* PedestalMesh;

	UPROPERTY(EditAnywhere)
		class USoundCue* SpawnSound;
private:
	FTimerHandle SpawnAITimer;

	UPROPERTY(EditAnywhere)
		float SpawnAITimeMin;

	UPROPERTY(EditAnywhere)
		float SpawnAITimeMax;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
