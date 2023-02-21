// Fill out your copyright notice in the Description page of Project Settings.


#include "Magazine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AMagazine::AMagazine()
{
	PrimaryActorTick.bCanEverTick = false;

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(MagazineMesh);
	MagazineMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	MagazineMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	MagazineMesh->SetSimulatePhysics(true);
	MagazineMesh->SetEnableGravity(true);
	MagazineMesh->SetNotifyRigidBodyCollision(true);
	MagazineEjectionImpulse = 200;
	bSoundPlayed = false;
	SetLifeSpan(15.0f);

}

// Called when the game starts or when spawned
void AMagazine::BeginPlay()
{
	Super::BeginPlay();

	MagazineMesh->OnComponentHit.AddDynamic(this, &AMagazine::OnHit);

	MagazineMesh->AddImpulse(GetActorForwardVector() * MagazineEjectionImpulse);
	
}

void AMagazine::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bSoundPlayed && MagazineSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MagazineSound, GetActorLocation());
		bSoundPlayed = true;
	}
}

