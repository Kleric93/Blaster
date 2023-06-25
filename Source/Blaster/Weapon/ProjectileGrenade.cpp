// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Blaster/BlasterTypes/Team.h"


AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	SpawnTrailSystem();
	StartDestroyTimer();
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileGrenade::OnSphereOverlap);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), OverlapSphere->GetScaledSphereRadius(), 50, FColor::Purple, true, -1, 0, 2);

}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}

void AProjectileGrenade::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this->GetOwner()) return;
	ABlasterCharacter* BlasterCharacterOwner = Cast<ABlasterCharacter>(this->GetOwner());
	ETeam OwnersTeam = BlasterCharacterOwner->GetTeam();
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	ETeam CharacterExplodingTeam = BlasterCharacter->GetTeam();
	
	bool bGrenadeCanGoBoom =
		BlasterCharacter &&
		BlasterCharacterOwner &&
		(OwnersTeam == ETeam::ET_RedTeam && CharacterExplodingTeam == ETeam::ET_BlueTeam) ||
		BlasterCharacter &&
		BlasterCharacterOwner &&
		(OwnersTeam == ETeam::ET_BlueTeam && CharacterExplodingTeam == ETeam::ET_RedTeam) ||
		BlasterCharacter&&
		BlasterCharacterOwner &&
		(OwnersTeam == ETeam::ET_NoTeam && CharacterExplodingTeam == ETeam::ET_NoTeam);

	if (BlasterCharacter && bGrenadeCanGoBoom)
	{
		ExplodeDamage();
		UE_LOG(LogTemp, Warning, TEXT("Damage Dealt %f"), Damage);

		DestroyTimerFinished();
		Destroyed();
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap detected, but not with a BlasterCharacter."));
	}
}

void AProjectileGrenade::Destroyed()
{
	ExplodeDamage();
	Super::Destroyed();
}
