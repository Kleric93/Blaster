// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = false;
	GrenadeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Grenade Mesh"));
	SetRootComponent(GrenadeMesh);

}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(GrenadeMesh))
	{
		GrenadeInstance = GrenadeMesh->GetAnimInstance();

		GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AGrenade::TypeSwitch, ExplosionTimer, false);
	}
}

void AGrenade::TypeSwitch()
{
	GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);

	switch (GrenadeType)
	{
	case EGrenadeType::EGT_None:
		break;

	case EGrenadeType::EGT_Seismic:

		Explode();

		break;

	case EGrenadeType::EGT_Smoke:

		Smoke();

		break;

	default:
		break;
	}
}

void AGrenade::Explode()
{
	UGameplayStatics::SpawnSoundAttached(ExplosionSound, GrenadeMesh);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GrenadeMesh->GetRelativeLocation(), GrenadeMesh->GetRelativeRotation());

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;

	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	const bool bHasSphereOverlapped = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GrenadeMesh->GetRelativeLocation(), DamageRadius, TraceObjects, nullptr, ActorsToIgnore, OutActors);

	if (bHasSphereOverlapped)
	{
		for (auto&& Out : OutActors)
			const bool bWasDamageApplied = UGameplayStatics::ApplyRadialDamage(GetWorld(), DamageAmount, GrenadeMesh->GetRelativeLocation(), DamageRadius,
				UDamageType::StaticClass(), ActorsToIgnore, this, GetInstigatorController(), false, ECollisionChannel::ECC_Visibility);
	}

	Destroy();
}

void AGrenade::Smoke()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GrenadeMesh->GetRelativeLocation(), GrenadeMesh->GetRelativeRotation());

	Destroy();
}

void AGrenade::OnGrenadeThrow(FVector ForwardVector)
{
	ForwardVector *= 2'500.F;

	GrenadeMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GrenadeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GrenadeMesh->SetSimulatePhysics(true);
	GrenadeMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GrenadeMesh->AddImpulse(ForwardVector);
}

