// Fill out your copyright notice in the Description page of Project Settings.


#include "BerserkPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"

void ABerserkPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UBuffComponent* Buff = BlasterCharacter->Getbuff();
		if (Buff)
		{
			if (BlasterCharacter->GetEquippedWeapon())
			{
				float BuffFireDelay = BlasterCharacter->GetEquippedWeapon()->FireDelay;
				Buff->BuffBerserk(BuffFireDelay, BerserkBuffTime);

				ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(BlasterCharacter->GetPlayerState());

				if (PS)
				{
					PS->OnBerserkBuffPickedUp(BerserkBuffTime);
				}
			}
		}
	}
	Destroy();
}