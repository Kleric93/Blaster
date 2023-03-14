// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = InitialSpeed;

}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
    Super::PostEditChangeProperty(Event);

    FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
    {
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->InitialSpeed = InitialSpeed;
            ProjectileMovementComponent->MaxSpeed = InitialSpeed;
        }
    }
}
#endif

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
    if (OwnerCharacter)
    {
        ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
        if (OwnerController)
        {
            // Calculate the distance traveled by the bullet
            float DistanceTraveled = (Hit.Location - OwnerController->GetPawn()->GetActorLocation()).Size() / 100;

            // Reduce the damage based on the distance traveled
            float DamageMultiplier = 1.f;
            if (DistanceTraveled > FullDamageDistance && DistanceTraveled <= LeastDamageDistance)
            {
                DamageMultiplier = FMath::Lerp(1.f, 0.1f, (DistanceTraveled - FullDamageDistance) / LeastDamageDistance);
            }
            else if (DistanceTraveled > FullDamageDistance)
            {
                DamageMultiplier = 0.1f;
            }

            float FinalDamage = Damage * DamageMultiplier;

            UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
            UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceTraveled);

            if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
            {
                UGameplayStatics::ApplyDamage( OtherActor, FinalDamage, OwnerController, this, UDamageType::StaticClass());
                Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
                return;
            }

            ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
            if (bUseServerSideRewind && OwnerCharacter->GetlagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
            {
                OwnerCharacter->GetlagCompensation()->ProjectileServerScoreRequest(
                    HitCharacter,
                    TraceStart,
                    InitialVelocity,
                    OwnerController->GetServerTime() - OwnerController->SingleTripTime
                );
            }
        }
    }
    /* Super:: goes last cause the original function will destroy the bullet,
    if we put it first, whatever comes after will never be reached.*/
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::BeginPlay()
{
    Super::BeginPlay();

    /*  FOR EDUCATIONAL PURPOSES
    *  
    FPredictProjectilePathParams Pathparams;
    Pathparams.bTraceWithChannel = true;
    Pathparams.bTraceWithCollision = true;
    Pathparams.DrawDebugTime = 5.f;
    Pathparams.DrawDebugType = EDrawDebugTrace::ForDuration;
    Pathparams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
    Pathparams.MaxSimTime = 4.f;
    Pathparams.ProjectileRadius = 5.f;
    Pathparams.SimFrequency = 30.f;
    Pathparams.StartLocation = GetActorLocation();
    Pathparams.TraceChannel = ECollisionChannel::ECC_Visibility;
    Pathparams.ActorsToIgnore.Add(this);

    FPredictProjectilePathResult PathResult;

    UGameplayStatics::PredictProjectilePath(this, Pathparams, PathResult);*/
}
