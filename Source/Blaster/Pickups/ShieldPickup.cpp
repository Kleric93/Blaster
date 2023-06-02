// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/BlasterComponents/BuffComponent.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter->GetShield() == BlasterCharacter->GetMaxShield()) return;
	if (BlasterCharacter)
	{
		UBuffComponent* Buff = BlasterCharacter->Getbuff();
		if (Buff)
		{
			Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);

			ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(BlasterCharacter->GetPlayerState());

			if (PS)
			{
				PS->OnShieldBuffPickedUp();
			}
		}
	}
	Destroy();
}