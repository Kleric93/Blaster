// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "EngineUtils.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Blaster/Blaster.h"
#include "Blaster/Character/BlasterAnimInstance.h"
#include "Blaster/Weapon/Projectile.h"
#include "Blaster/Grenade/Grenade.h"
#include "Blaster/Weapon/Shotgun.h"
#include "Blaster/Pickups/TeamsFlag.h"
#include "Blaster/HUD/OverheadWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "Blaster/BlasterUserSettings.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"





UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 700.f;
	AimWalkSpeed = 500.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, Grenades);
	DOREPLIFETIME(UCombatComponent, bLocallyReloading);
	DOREPLIFETIME(UCombatComponent, bIsSliding);
	DOREPLIFETIME(UCombatComponent, bIsProne);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Settings == nullptr)
	{
		Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
	}

	UpdateHUDGrenades();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		FHitResult ObjectHitResult;
		TraceForObjects();
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);

		if (Settings->GetIsUsingKBM() == false)
		{
			UpdateAim(DeltaTime);
		}
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (EquippedWeapon == nullptr) return;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if (Character && Character->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = .75f;

			switch (EquippedWeapon->FireType)
			{
			case EFireType::EFT_Projectile:
				FireProjectileWeapon();
				break;

			case EFireType::EFT_HitScan:
				FireHitScanWeapon();
				break;

			case EFireType::EFT_Shotgun:
				FireShotgun();
				break;
			}
		}
		StartFireTimer();
	}
}

void UCombatComponent::FireProjectileWeapon()
{
	if (EquippedWeapon && Character)
	{
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget, EquippedWeapon->FireDelay);
	}
}
void UCombatComponent::FireHitScanWeapon()
{
	if (EquippedWeapon && Character)
	{
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		ServerFire(HitTarget, EquippedWeapon->FireDelay);
	}
}
void UCombatComponent::FireShotgun()
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeapon);
	if (Shotgun && Character)
	{
		TArray<FVector_NetQuantize> HitTargets;
		Shotgun->ShotgunTraceEndWithScatter(HitTarget, HitTargets);
		if (!Character->HasAuthority()) LocalShotgunFire(HitTargets);
		ServerShotgunFire(HitTargets, EquippedWeapon->FireDelay);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	ReloadEmptyWeapon();
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	MulticastFire(TraceHitTarget);
}

bool UCombatComponent::ServerFire_Validate(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	if (EquippedWeapon)
	{
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	LocalFire(TraceHitTarget);
}

void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	MulticastShotgunFire(TraceHitTargets);

}

bool UCombatComponent::ServerShotgunFire_Validate(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (EquippedWeapon)
	{
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}

void UCombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;

	LocalShotgunFire(TraceHitTargets);
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeapon);
	if (Shotgun == nullptr || Character == nullptr) return;
	if (CombatState == ECombatState::ECS_Reloading || CombatState == ECombatState::ECS_Unoccupied)
	{
		bLocallyReloading = false;
		Character->PlayFireMontage(bAiming);
		Shotgun->FireShotgun(TraceHitTargets);
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	SetWeaponTypeOnHUD();
	AttachActorToRighthand(EquippedWeapon);
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(WeaponToEquip);
	ReloadEmptyWeapon();
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetOwner(Character);
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(WeaponToEquip);
	PlayEquipWeaponSound(WeaponToEquip);
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonpressed;
	}
}

void UCombatComponent::SwapWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied || Character == nullptr) return;

	if (!bAiming)
	{
		bAiming = false;
		if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_M4AZ)
		{
			Character->ShowM4ScopeWidget(false);
		}
		else if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
		{
			Character->ShowSniperScopeWidget(false);
		}
		Character->PlaySwapMontage();
		Character->bFinishedSwapping = false;
		CombatState = ECombatState::ECS_SwappingWeapons;

		AWeapon* TempWeapon = EquippedWeapon;
		EquippedWeapon = SecondaryWeapon;
		SecondaryWeapon = TempWeapon;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		Character->PlaySwapMontage();
		AWeapon* TempWeapon = EquippedWeapon;
		EquippedWeapon = SecondaryWeapon;
		SecondaryWeapon = TempWeapon;
		EquipPrimaryWeapon(WeaponToEquip);
		EquipSecondaryWeapon(TempWeapon);
		//EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::EquipFlag(class ATeamsFlag* FlagToEquip)
{
	if (FlagToEquip == nullptr) return;
	EquippedFlag = FlagToEquip;
	EquippedFlag->SetFlagStateOD(EFlagState::EFS_Equipped);
	AttachFlagToBackpack(EquippedFlag);
	EquippedFlag->SetOwner(Character);
	EquippedFlag->GetFlagMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
}

void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UCombatComponent::AttachActorToRighthand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr || EquippedWeapon == nullptr) return;
	bool bUsePistolSocket = 
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SMG;
	FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName(SocketName));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToBackpack(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* BackPackSocket = Character->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (BackPackSocket)
	{
		BackPackSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachFlagToBackpack(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* BackPackSocket = Character->GetMesh()->GetSocketByName(FName("FlagSocket"));
	if (BackPackSocket)
	{
		BackPackSocket->AttachActor(ActorToAttach, Character->GetMesh());

		UGameplayStatics::PlaySound2D(
			GetWorld(),
			FlagPickupSound
			);
	}
}

void UCombatComponent::OnRep_EquippedFlag()
{
	if (EquippedFlag && Character)
	{
		EquippedFlag->SetFlagState(EFlagState::EFS_Equipped);
		AttachFlagToBackpack(EquippedFlag);
	}
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::PlayEquipWeaponSound(AWeapon* WeaponToEquip)
{
	if (Character && WeaponToEquip && EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponToEquip->EquipSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::ReloadEmptyWeapon()
{
	if (EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}


void UCombatComponent::Reload()
{
	bool bCanReload = CarriedAmmo > 0 && 
		CombatState == ECombatState::ECS_Unoccupied &&
		EquippedWeapon && !EquippedWeapon->IsFull() && 
		!bLocallyReloading;

	if (bCanReload)
	{
		ServerReload();
		HandleReload();
		bLocallyReloading = true;

		FTimerHandle ReloadStuckTimer;
		GetWorld()->GetTimerManager().SetTimer(
			ReloadStuckTimer, 
			this, 
			&UCombatComponent::DoubleCheckReloadingStateChange, 
			5.f, 
			false);
	}
}

void UCombatComponent::DoubleCheckReloadingStateChange()
{
	if (bLocallyReloading && CombatState == ECombatState::ECS_Reloading)
	{
		CombatState = ECombatState::ECS_Unoccupied;
		bLocallyReloading = false;
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_Reloading;
	//if (bAiming) bAiming = false;
	if (!Character->IsLocallyControlled()) HandleReload();
}

void UCombatComponent::HandleReload()
{
	if (Character)
	{
		Character->PlayReloadMontage();
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	bLocallyReloading = false;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bFireButtonPressed)
	{
		Fire();
	}

}

void UCombatComponent::FinishSwap()
{
	if (Character && Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if (Character) Character->bFinishedSwapping = true;
}

void UCombatComponent::FinishSwapAttachWeapons()
{

	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeapon);

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachActorToRighthand(EquippedWeapon);
	EquippedWeapon->SetHUDAmmo();
	PlayEquipWeaponSound(EquippedWeapon);
	SetWeaponTypeOnHUD();
	UpdateCarriedAmmo();
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AddAmmo(ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(1);
	bCanFire = true;
	if (EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::JumpToShotgunEnd()
{
	// Jump to shotgunEnd Section
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->GetReloadMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
	}
}

void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToRighthand(EquippedWeapon);
}

void UCombatComponent::Launchgrenade()
{
	ShowAttachedGrenade(false);
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (Character && GrenadeClass && Character->GetAttachedGrenade())
	{
		const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
		FVector ToTarget = Target - StartingLocation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
	
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(
				GrenadeClass,
				StartingLocation,
				ToTarget.Rotation(),
				SpawnParams
				);
		}
	}
}

void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_Reloading:
		if (Character && !Character->IsLocallyControlled()) HandleReload();
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachActorToLeftHand(EquippedWeapon);
			ShowAttachedGrenade(true);
		}
		break;
	case ECombatState::ECS_SwappingWeapons:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlaySwapMontage();
		}
		break;
	case ECombatState::ECS_MAX:
		break;
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedWeapon->GetmagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;
	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon); 
		ShowAttachedGrenade(true);
	}
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}


void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
	}
}

void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(bShowGrenade);
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachActorToRighthand(EquippedWeapon);

		SetWeaponTypeOnHUD();
		PlayEquipWeaponSound(EquippedWeapon);
		EquippedWeapon->SetHUDAmmo();
	}
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (SecondaryWeapon && Character)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackpack(SecondaryWeapon);
		PlayEquipWeaponSound(EquippedWeapon);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::TraceForObjects()
{
	
	// Hide all overhead widgets initially
	for (TActorIterator<ABlasterCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ABlasterCharacter* BlasterCharacter = *ActorItr;
		
		//BlasterCharacter->GetOverheadWidgetComponent()->SetVisibility(false);

		if (BlasterCharacter->IsLocallyControlled() && Settings->GetbLocalPlayerOverheadWidgetVisibility() == true)
		{
			BlasterCharacter->GetOverheadWidgetComponent()->SetVisibility(true);
		}
		else if (BlasterCharacter->IsLocallyControlled() && Settings->GetbLocalPlayerOverheadWidgetVisibility() == false)
		{
			BlasterCharacter->GetOverheadWidgetComponent()->SetVisibility(false);
		}
	}


	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 500.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECollisionChannel::ECC_GameTraceChannel3 // Common channel for both walls and widgets
		);

		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor == nullptr) return;

			ABlasterCharacter* HitBlasterCharacter = Cast<ABlasterCharacter>(HitActor);
			if (HitBlasterCharacter == nullptr) { return; }


			if (HitBlasterCharacter && HitResult.GetComponent() == HitBlasterCharacter->GetOverheadWidgetBoxComponent())
			{

				HitBlasterCharacter->GetOverheadWidgetComponent()->SetVisibility(true);
				// Calculate the distance from the player to the widget
				float Distance = FVector::Dist(HitBlasterCharacter->GetActorLocation(), Character->GetActorLocation());

				// Clamp the distance between the min and max distances
				float ClampedDistance = FMath::Clamp(Distance, WidgetMinDistance, WidgetMaxDistance);

				// Normalize the clamped distance to a value between 0 and 1
				float NormalizedDistance = (ClampedDistance - WidgetMinDistance) / (WidgetMaxDistance - WidgetMinDistance);

				// Invert NormalizedDistance to make text smaller when farther away
				NormalizedDistance = 1 - NormalizedDistance;

				// Interpolate between the min and max size based on the normalized distance
				float SizeFloat = FMath::Lerp(WidgetMinSize, WidgetMaxSize, NormalizedDistance);

				// Convert the float size to integer
				int32 Size = FMath::RoundToInt(SizeFloat);

				// Set the size of the widget
				UOverheadWidget* OHWidget = Cast<UOverheadWidget>(HitBlasterCharacter->GetOverheadWidgetComponent()->GetUserWidgetObject());
				if (OHWidget)
				{
					OHWidget->SetTextSize(Size);
					ETeam TeamToSet = HitBlasterCharacter->GetTeam();
					OHWidget->ChangeOWColor(TeamToSet);
					//UE_LOG(LogTemp, Warning, TEXT("Setting Draw Size: %f"), SizeFloat);
					//UE_LOG(LogTemp, Warning, TEXT("Distance from other player: %f"), Distance);
				}
				//UE_LOG(LogTemp, Warning, TEXT("Setting timer..."));

				GetWorld()->GetTimerManager().ClearTimer(HitBlasterCharacter->HideWidgetTimerHandle); // Clear any existing timer
				GetWorld()->GetTimerManager().SetTimer(
					HitBlasterCharacter->HideWidgetTimerHandle,
					[HitBlasterCharacter]()
					{
						//UE_LOG(LogTemp, Warning, TEXT("Timer expired!"));
						HitBlasterCharacter->GetOverheadWidgetComponent()->SetVisibility(false);
					},
					HideWidgetTimer, false);

			}
		}
	}
}

void UCombatComponent::TraceForAimAssist()
{
	TargetAimDirection = FVector::ZeroVector;

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 400.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_EngineTraceChannel4
		);

		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor == nullptr) return;

			ABlasterCharacter* HitBlasterCharacter = Cast<ABlasterCharacter>(HitActor);
			if (HitBlasterCharacter == nullptr) { return; }

			if (HitResult.GetComponent() != HitBlasterCharacter->GetAimAssistSphereComponent())
			{
				return;
			}

			if (HitBlasterCharacter && HitResult.GetComponent() == HitBlasterCharacter->GetAimAssistSphereComponent())
			{
				// This should be the world location of your target's center
				FVector TargetCenter = HitBlasterCharacter->GetAimAssistSphereComponent()->GetComponentLocation();

				TargetAimDirection = (TargetCenter - CrosshairWorldPosition).GetSafeNormal();
			}
		}
	}
}

void UCombatComponent::UpdateAim(float DeltaTime)
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController == nullptr) return;

	// Step 1: Get player input
	float InputPitch = Character->GetLastLookInput().Y;
	float InputYaw = Character->GetLastLookInput().X;

	// Step 3: Use your existing line trace logic to find the enemy
	TraceForAimAssist();

	// Step 4: If the line trace hits the enemy's aim assist collider, calculate the desired pitch and yaw
	if (!TargetAimDirection.IsNearlyZero())
	{
		FRotator DesiredAimRot = TargetAimDirection.Rotation();
		FRotator CurrentAimRot = PlayerController->GetControlRotation();

		// Step 5: Calculate the difference between the desired and current rotations
		FRotator DeltaRot = (DesiredAimRot - CurrentAimRot).GetNormalized();

		// Step 6: Calculate the squared length of the rotation axis
		float RotationLengthSquared = DeltaRot.Vector().SizeSquared();

		// Step 7: Calculate the interpolation speed based on the squared length of the rotation axis
		float InterpolationSpeed = FMath::Clamp(RotationLengthSquared / 180.f, 0.f, 1.f) * AimAssistSpeed;

		// Step 8: Interpolate the current pitch and yaw of the player controller towards the desired pitch and yaw
		FRotator NewAimRot = CurrentAimRot + DeltaRot * InterpolationSpeed * DeltaTime;

		// Apply the new pitch and yaw to the player controller
		PlayerController->SetControlRotation(NewAimRot);
	}
}


void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}
			// calculate crosshair spread


			// [0, 600] -> [0, 1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 20.f);

			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}
	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
		//UE_LOG(LogTemp, Error, TEXT("Reload() is being called!!!!!!!!"))
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	bAiming = bIsAiming;
	CombatState = ECombatState::ECS_Unoccupied;
	ServerSetAiming(bIsAiming);
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	bool bCanReduceAimSpeed =
		Character &&
		BuffComponent->GetbIsSpeedBuffActive() == false &&
		!bIsSliding &&
		!bIsProne;

	if (bCanReduceAimSpeed)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
		//UE_LOG(LogTemp, Warning, TEXT("MaxWalkSpeed is now: %f"), Character->GetCharacterMovement()->MaxWalkSpeed);
	}
	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
		Controller->SetCharacterOverlayVisibility(!bIsAiming);
	}
	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_M4AZ)
	{
		Character->ShowM4ScopeWidget(bIsAiming);
		Controller->SetCharacterOverlayVisibility(!bIsAiming);

	}
	if (Character->IsLocallyControlled()) bAimButtonpressed = bIsAiming;
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	bAiming = bIsAiming;
	bool bCanReduceAimSpeed =
		Character &&
		BuffComponent->GetbIsSpeedBuffActive() == false &&
		!bIsSliding &&
		!bIsProne;

	if (bCanReduceAimSpeed)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	if (!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun) return true;
	if (bLocallyReloading) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	bool bJumpToShotgunEnd = 
		CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr &&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun &&
		CarriedAmmo == 0;
	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_M4AZ, StartingM4AZAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSniperAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
}

void UCombatComponent::SetWeaponTypeOnHUD()
{
	if (Controller && EquippedWeapon)
	{
		Controller->SetHUDWeaponType(EquippedWeapon->GetWeaponType());
	}
}

void UCombatComponent::OnRep_HandleReload()
{
	if (EquippedWeapon->IsEmpty())
	{
		if (Character)
		{
			Character->PlayReloadMontage();
		}
	}
}

void UCombatComponent::OnRep_Sliding()
{
	bIsSliding = true;
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	if (Character && BuffComponent->GetbIsSpeedBuffActive() == false)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = SlidingSpeed;
	}
	GetWorld()->GetTimerManager().SetTimer(SlidingTimerHandle, this, &UCombatComponent::StopSliding, 1.f, false);
}

void UCombatComponent::StartSliding()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		ServerStartSliding();
	}
	else
	{
		MulticastStartSliding();
	}
}

void UCombatComponent::StopSliding()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		ServerStopSliding();
	}
	else
	{
		MulticastStopSliding();
	}
}

void UCombatComponent::ServerStartSliding_Implementation()
{
	MulticastStartSliding();
}

void UCombatComponent::ServerStopSliding_Implementation()
{
	MulticastStopSliding();
}

void UCombatComponent::MulticastStartSliding_Implementation()
{
	bIsSliding = true;
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	if (Character && BuffComponent->GetbIsSpeedBuffActive() == false)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = SlidingSpeed;
	}
	GetWorld()->GetTimerManager().SetTimer(SlidingTimerHandle, this, &UCombatComponent::StopSliding, 1.f, false);
}

void UCombatComponent::MulticastStopSliding_Implementation()
{
	bIsSliding = false;
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	if (Character && BuffComponent->GetbIsSpeedBuffActive() == false)
	{
		if (bIsProne == true)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = ProneSpeed;
		}
		else
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		}
	}
}

void UCombatComponent::OnRep_Prone()
{
	bIsProne = false;
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	if (Character && BuffComponent->GetbIsSpeedBuffActive() == false)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		//UE_LOG(LogTemp, Warning, TEXT("MaxWalkSpeed is now: %f"), Character->GetCharacterMovement()->MaxWalkSpeed);
	}
}

void UCombatComponent::StartProne()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		ServerStartProne();
	}
	else
	{
		MulticastStartProne();
	}
	//GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void UCombatComponent::StopProne()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		ServerStopProne();
	}
	else
	{
		MulticastStopProne();
	}
}

void UCombatComponent::ServerStartProne_Implementation()
{
	MulticastStartProne();
}

void UCombatComponent::ServerStopProne_Implementation()
{
	MulticastStopProne();
}

void UCombatComponent::MulticastStartProne_Implementation()
{
	bIsProne = true;
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	if (Character && BuffComponent->GetbIsSpeedBuffActive() == false)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = ProneSpeed;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void UCombatComponent::MulticastStopProne_Implementation()
{
	bIsProne = false;
	UBuffComponent* BuffComponent = Cast<UBuffComponent>(Character->Getbuff());
	if (Character && BuffComponent->GetbIsSpeedBuffActive() == false)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}