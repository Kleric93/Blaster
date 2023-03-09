

#include "AmmoPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	//ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(GetControlle)
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UCombatComponent* Combat = BlasterCharacter->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	/* // test for grenade pickups, failed miserably
	ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(BlasterCharacter->GetController());
	if (PlayerController)
	{
		UCombatComponent* Combat = BlasterCharacter->GetCombat();
		int32 CurrentGrenades = Combat->GetGrenades();
		PlayerController->SetHUDGrenades(CurrentGrenades + 1);
	}*/

	Destroy();
}
