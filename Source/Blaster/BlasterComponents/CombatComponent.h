// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "CombatComponent.generated.h"

class UBlasterUserSettings;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhantomStrideActivated, float, Duration);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )

class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;

	FOnPhantomStrideActivated OnPhantomStrideActivated;

	UPROPERTY()
		UBlasterUserSettings* Settings;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);

	void EquipBladeStart();

	UFUNCTION(Server, Reliable)
	void Server_EquipBladeStart();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_EquipBladeStart();

	void SheatheBladeAndPickMainWeapon();

	void EquipFlag(class ATeamsFlag* FlagToEquip);

	void AttachFlagToBackpack(AActor* ActorToAttach);
	void SwapWeapon();

	void Reload();

	UFUNCTION(BlueprintCallable)
		void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapons();

	void FireButtonPressed(bool bPressed);
	
	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable)
	void Launchgrenade();

	UFUNCTION(Server, Reliable)
		void Server_PhantomStride();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PhantomStride();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_PhantomStrideFinished();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PhantomStrideFinishedAnim();

	UFUNCTION(NetMulticast, Reliable)
		void SpawnBlade();
	
	UFUNCTION(Server, Reliable)
	void Server_MeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MeleeAttack();

	UFUNCTION()
	void StopMelee();

	UFUNCTION()
	void ReEnableInput();

	UFUNCTION()
	void FinishPhantomStrideTrail();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	UFUNCTION()
	void OnRep_HandleReload();

	UPROPERTY(ReplicatedUsing = OnRep_HandleReload)
	bool bLocallyReloading = false;

	UFUNCTION()
	void OnRep_EquippedFlag();

#pragma region Sliding Functions

	void StartSliding();

	void StopSliding();

	UPROPERTY(ReplicatedUsing = OnRep_Sliding)
		bool bIsSliding = false;

	UFUNCTION()
		void OnRep_Sliding();

	UFUNCTION(Server, Reliable)
		void ServerStartSliding();

	UFUNCTION(Server, Reliable)
		void ServerStopSliding();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastStartSliding();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastStopSliding();

	UPROPERTY(EditAnywhere)
		float SlidingSpeed = 1600.f;

	FTimerHandle SlidingTimerHandle;

#pragma endregion Sliding Functions

#pragma region Prone Functions

	void StartProne();
	void StopProne();


	UFUNCTION(Server, Reliable)
	void ServerStartProne();

	UFUNCTION(Server, Reliable)
	void ServerStopProne();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartProne();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopProne();

	UPROPERTY(ReplicatedUsing = OnRep_Prone)
	bool bIsProne = false;

	UFUNCTION()
		void OnRep_Prone();

	UPROPERTY(EditAnywhere)
		float ProneSpeed = 100.f;

#pragma endregion Prone Functions

protected:
	void SetAiming(bool bIsAiming);

	virtual void BeginPlay() override;
	

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
		void OnRep_SecondaryWeapon();

	UFUNCTION()
	void OnRep_TertiaryWeapon();

	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void FireBlade();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void TraceForObjects();

	void TraceForAimAssist();

	void UpdateAim(float DeltaTime);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();

	void SetWeaponTypeOnHUD();

	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UFUNCTION()
	void DoubleCheckReloadingStateChange();

	/*UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;*/

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectile> GrenadeClass;

	//void ThrowSmokeGrenade();
	//UFUNCTION(Server, Reliable)
	//void ServerThrowSmokeGrenade();

	void DropEquippedWeapon();
	void AttachActorToRighthand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToBackpackUlt(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void ShowAttachedGrenade(bool bShowGrenade);
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void EquipTertiaryWeapon(AWeapon* WeaponToEquip);

private:

	UPROPERTY()
	class ABlasterCharacter* Character;

	UPROPERTY()
	class ABlasterPlayerController* Controller;

	UPROPERTY()
	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedFlag)
	class ATeamsFlag* EquippedFlag;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_TertiaryWeapon)
	AWeapon* TertiaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	bool bAimButtonpressed = false;

	bool bInitiatingPhantomStride;
	bool bEndingPhantomStride;

	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	bool bIsUsingMelee = false;

	UPROPERTY(EditAnywhere)
	class USoundCue* FlagPickupSound;

	/*
	** HUD and Crosshair
	*/

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	/*
	*** Aiming and FOV
	*/

	// field of view when not aiming. set to the camera's base FOV in begin play
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	//
	//// Automatic Fire
	//

	FTimerHandle FireTimer;
	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	// carried ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	void InitializeCarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 200;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingM4AZAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 8;

	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 4;

	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo = 0;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> PhantomStrideStartShake;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShakeBase> DeathCameraShake;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState, VisibleAnywhere)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();

	void UpdateShotgunAmmoValues();

	UPROPERTY(ReplicatedUsing = OnRep_Grenades, EditAnywhere)
		int32 Grenades = 2;

	UFUNCTION()
		void OnRep_Grenades();

	UPROPERTY(EditAnywhere)
		int32 MaxGrenades = 6;

	void UpdateHUDGrenades();

	UPROPERTY(EditAnywhere)
	float WidgetMinSize = 10;

	UPROPERTY(EditAnywhere)
	float WidgetMaxSize = 40;

	UPROPERTY(EditAnywhere)
	float WidgetMinDistance = 100;

	UPROPERTY(EditAnywhere)
	float WidgetMaxDistance = 5000;

	UPROPERTY(EditAnywhere)
	float HideWidgetTimer = 0.5f;

	UPROPERTY()
	FVector TargetAimDirection;
	UPROPERTY()
	FVector CurrentAimDirection;

	UPROPERTY(EditAnywhere)
	float AimAssistSpeed = 1000.f;

	UPROPERTY(EditAnywhere)
		float AimAssistmultiplierControl = 30.f;

	UPROPERTY(EditAnywhere)
	float AimAssistTraceStartLocation = 300.f;

	UPROPERTY(EditAnywhere)
	float BladeSpawnerTimer = 1.3f;

	UPROPERTY(EditAnywhere)
	float PhantomStrideAbilityDuration = 20.f;

	UPROPERTY(EditAnywhere)
		class USoundCue* BladeHum;

	UPROPERTY()
		class UAudioComponent* BladeHumComponent;

	UPROPERTY(EditAnywhere)
		USoundAttenuation* BladeHumAttenuation;

public:	

	FORCEINLINE bool IsAiming() const { return bAiming; }

	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetbInitiatingPhantomStride() const { return bInitiatingPhantomStride; }
	FORCEINLINE bool GetbEndingPhantomStride() const { return bEndingPhantomStride; }
	FORCEINLINE float GetPhantomStrideAbilityDuration() const { return PhantomStrideAbilityDuration; }
	bool ShouldSwapWeapons();
	

};
