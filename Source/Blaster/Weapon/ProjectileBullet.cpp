// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"


AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AController* OwnerController = OwnerCharacter->Controller;
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

            UGameplayStatics::ApplyDamage(
                OtherActor,
                FinalDamage,
                OwnerController,
                this,
                UDamageType::StaticClass());

            UE_LOG(LogTemp, Warning, TEXT("Final Damage Dealt: %f"), FinalDamage);
            UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceTraveled);
        }
    }

    /* Super:: goes last cause the original function will destroy the bullet,
    if we put it first, whatever comes after will never be reached.*/
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
