// Fill out your copyright notice in the Description page of Project Settings.


#include "AISpawner.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blaster/Character/BlasterCharacter.h"

#include "Blaster/PlayerController/BlasterPlayerController.h"

// Sets default values
AAISpawner::AAISpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	PedestalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pedestal Mesh"));
	PedestalMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnAITimerFinished();

	PedestalParticlesComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystemParticles, GetActorLocation());
	if (PedestalParticlesComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Particles DEactivated BeginPlay"));

		PedestalParticlesComponent->Deactivate();
	}

	// Check if the pedestal mesh is currently using the default material
	if (PedestalMesh->GetMaterial(0) == DefaultPedestalMaterial)
	{
		// If it is, swap it to the special material
		PedestalMesh->SetMaterial(0, DefaultPedestalMaterial);
		SetPedestalDefaultMaterial();
	}
}

// Called every frame
void AAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAISpawner::SetPedestalDefaultMaterial()
{
	// Set the material on the server and replicate it to clients
	PedestalMesh->SetMaterial(0, DefaultPedestalMaterial);
}

void AAISpawner::SetPedestalOnMaterial()
{
	// Set the material on the clients as well
	PedestalMesh->SetMaterial(0, WeaponSpawnedPedestalMaterial);

	PedestalParticlesComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystemParticles, GetActorLocation());
	if (PedestalParticlesComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Particles activated in SpawnWeaponTimerFInished"));
		PedestalParticlesComponent->Activate();
	}
}

void AAISpawner::PlaySpawnSound(USoundCue* Sound, FVector Location)
{
	// Play the sound on all clients
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location);
}

void AAISpawner::SpawnAI()
{
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += 100.0f; // Adjust this offset as needed
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedAI = GetWorld()->SpawnActor<ABlasterCharacter>(CharacterClass, FTransform(SpawnLocation), SpawnParams);

	if (SpawnedAI)
	{
		// Get a reference to the mesh (modify this to match your actual setup)
		USkeletalMeshComponent* MeshComponent = SpawnedAI->GetMesh();

		if (MeshComponent)
		{
			// Enable physics
			//MeshComponent->SetSimulatePhysics(true);
			MeshComponent->SetEnableGravity(true); // Ensure gravity is enabled
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
		}

		SpawnedAI->OnDestroyed.AddDynamic(this, &AAISpawner::StartSpawnAITimer);
	}

	SpawnAIDelegate();
}

void AAISpawner::SpawnAIDelegate()
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(SpawnedAI);

	if (BlasterCharacter)
	{
		OnAISpawned.Broadcast(this);
	}
}

void AAISpawner::SpawnAITimerFinished()
{
	SpawnAI();

	PlaySpawnSound(SpawnSound, GetActorLocation());
	SetPedestalOnMaterial();
}

void AAISpawner::StartSpawnAITimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnAITimeMin, SpawnAITimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnAITimer,
		this,
		&AAISpawner::SpawnAITimerFinished,
		SpawnTime
	);
	SetPedestalDefaultMaterial();
}

