// Fill out your copyright notice in the Description page of Project Settings.


#include "Blade.h"
#include "Engine/SkeletalmeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Blaster/Blaster.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "WeaponTypes.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/SKeletalMeshSocket.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


ABlade::ABlade()
{

}

void ABlade::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // If we are teleporting, interpolate our position towards the target location
    
    if (bIsTeleporting)
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        FVector CurrentLocation = OwnerPawn->GetActorLocation();
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 5.0f); // You can adjust the speed as needed

        // Set the new location
        OwnerPawn->SetActorLocation(NewLocation);

        // If we are close enough to the target location, stop teleporting
        //if (FVector::DistSquared(CurrentLocation, TargetLocation) < 300.f) // 10 Unreal units (1m squared) away from the final location
        float Distance = (TargetLocation - OwnerPawn->GetActorLocation()).Size() / 100.f;
        if(Distance <= 2.5f)
        {
            bIsTeleporting = false;
        }
    }
}

void ABlade::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return;
    AController* InstigatorController = OwnerPawn->GetController();

    const USkeletalMeshSocket* BladeSocket = GetWeaponMesh()->GetSocketByName("BladeSocket");
    if (BladeSocket)
    {
        FTransform SocketTransform = BladeSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();

        FHitResult FireHit;
        WeaponTraceHit(Start, HitTarget, FireHit);

        float BladeDamage = 1000.f;

        ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
        if (BlasterCharacter && InstigatorController)
        {
            bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
            if (HasAuthority() && bCauseAuthDamage)
            {
                // Calculate direction from the attacker to the target
                //FVector AttackDirection = (BlasterCharacter->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();

                // Calculate the new position 1 meter (or 100 Unreal units) before the target
                //NewPosition = BlasterCharacter->GetActorLocation() - AttackDirection * 100;
                if (FireHit.GetActor() != BlasterCharacter)
                {
                    ABlasterCharacter* OwnerBlasterCharacter = Cast<ABlasterCharacter>(OwnerPawn);
                }
                // Set target location for teleporting
                TargetLocation = BlasterCharacter->GetActorLocation();

                // Start teleporting
                bIsTeleporting = true;

                UE_LOG(LogTemp, Warning, TEXT("OwnerPawn Name: %s, Location: %s"), *OwnerPawn->GetName(), *OwnerPawn->GetActorLocation().ToString());
                UE_LOG(LogTemp, Warning, TEXT("BlasterCharacter Name: %s, Location: %s"), *BlasterCharacter->GetName(), *BlasterCharacter->GetActorLocation().ToString());

                // Apply damage to the hit player after the owner pawn has reached the destination
                FTimerHandle UnusedHandle;
                GetWorldTimerManager().SetTimer(UnusedHandle, [this, BlasterCharacter, BladeDamage, InstigatorController]()
                    {
                        UGameplayStatics::ApplyDamage(
                            BlasterCharacter,
                            BladeDamage,
                            InstigatorController,
                            this,
                            UDamageType::StaticClass()
                        );
                    }, .5f, false);
            }
        }
    }
}


void ABlade::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		FCollisionQueryParams TraceParams(FName(TEXT("FireTrace")), true, OwnerPawn);
		TraceParams.AddIgnoredActor(this);

		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_GameTraceChannel3,
			TraceParams
		);
		if (OutHit.bBlockingHit)
		{
			End = OutHit.ImpactPoint;
        }
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;

			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OutHit.GetActor());
		}
		else
		{
			OutHit.ImpactPoint = End;
		}
	}
}

void ABlade::MulticastBladeDamage_Implementation(const FString& PlayerName, int32 HitscanDamage)
{
	OnBladeDamage.Broadcast(PlayerName, HitscanDamage);
}

void ABlade::MulticastSetBladeMaterialOff_Implementation()
{
    // Set the material on the server and replicate it to clients
    GetWeaponMesh()->SetMaterial(1, BladeMaterialOff);

}

void ABlade::MulticastSetBladeMaterialOn_Implementation()
{
    // Set the material on the server and replicate it to clients
    GetWeaponMesh()->SetMaterial(1, BladeMaterialOn);

}


void ABlade::MulticastSetBladeParticlesOn_Implementation()
{
    if (BladeSystemParticles == nullptr) return;
    if (BladeSystemComponent == nullptr)
    {
        BladeSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            BladeSystemParticles,
            GetWeaponMesh(),
            FName(),
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            false
        );
    }
    if (BladeSystemComponent)
    {
        BladeSystemComponent->Activate();
    }
    //BladeSystemComponent->SetAsset(BladeSystemParticles);

    // Calculate the vector between the two sockets
    FVector Start = GetWeaponMesh()->GetSocketLocation("Blade_Start");
    FVector End = GetWeaponMesh()->GetSocketLocation("Blade_End");
    FVector ParticleSystemLength = End - Start;

    // Set the length of the particle system to match the blade
    BladeSystemComponent->SetNiagaraVariableVec3("ParticleSystemLength", ParticleSystemLength);
    BladeSystemComponent->Activate();
}

void ABlade::MulticastSetBladeParticlesOff_Implementation()
{
    BladeSystemComponent->Deactivate();
}

