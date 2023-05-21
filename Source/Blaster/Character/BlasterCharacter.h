// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "InputActionValue.h"
#include "BlasterCharacter.generated.h"

class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	//
	/// Play Montages
	//
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapMontage();

	virtual void OnRep_ReplicatedMovement() override;
	void Elim(bool PlayerLeftGame);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	virtual void Destroyed() override;

	void RespawnSentence();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowM4ScopeWidget(bool bShowScope);
	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();

	void UpdateHUDFlag();

	UPROPERTY()
	class ATeamsFlag* Flag;

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);

	UFUNCTION()
	void UpdateMinimapRenderTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPaperSpriteComponent* HeadIconSpriteComponent;

	UPROPERTY(EditAnywhere)
	FVector HeadIconLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSprite* DefaultPlayerIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
		class UPaperSprite* RedTeamPlayerIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
		class UPaperSprite* BlueTeamPlayerIcon;
	
	void SetHeadIcon();

protected:

	//
	/// Enhanced Input
	//
	UPROPERTY(EditAnywhere, Category = Input, meta = (ClampMin = "0.1", ClampMax = "1"))
	float BaseTurnSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, Category = Input)
	float TurnSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrowGrenadeAction;

	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	void EquipButtonPressed();
	void SwapButtonPressed();
	void CrouchButtonPressed();
	void ProneButtonPressed();
	void Aim(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void ReloadButtonPressed();
	void GrenadeButtonPressed();



	virtual void BeginPlay() override;
	void ARMagazineAnimation();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void TurnInPlace(float DeltaTime);
	void PlayHitReactMontage();
	// Poll for any relevant classes and initialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);
	//void SmokeGrenadeButtonPressed();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void SetSpawnPoint();
	void OnPlayerStateInitialized();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UPROPERTY(EditAnywhere)
	class UTextureRenderTarget2D* MinimapRenderTarget;

	//
	/// Hit boxes used for Server Side Rewind
	//
	#pragma region HitBoxes

	UPROPERTY(EditAnywhere)
		UBoxComponent* head;

	UPROPERTY(EditAnywhere)
		UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
		UBoxComponent* blankett;

	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_r;

	#pragma endregion 

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* OverheadBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USceneCaptureComponent2D* MinimapCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	//
	/// BlasterComponents
	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerSwapButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	void HideCharacterAndWeaponsIfScopingOrCameraClose();

	AWeapon* StartingWeapon;

	//
	/// Animation Montages
	//

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	USceneComponent* HandSceneComponent;


	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* SwapMontage;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	//
	/// Player Health
	//

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "PlayerStats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	//
	/// Player Shield
	//

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "PlayerStats")
		float Shield = 0.f;

	UFUNCTION()
		void OnRep_Shield(float LastShield);

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	bool bLeftGame = false;
	bool bHasAlreadyJoined;


	//
	/// Dissolve Effect
	//
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	// Dynamic Instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the blueprint, used with the dynamic material instance
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;

	//
	/// Team Colors
	//

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* OriginalDissolveMatInst;
	
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* RedDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* RedMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* BlueDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* BlueMaterial;

	//
	/// Elim Effects
	//

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY(EditAnywhere)
	USoundCue* RespawnSentences;

	UPROPERTY(EditAnywhere)
	USoundCue* DroppedFlag;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

	//
	/// Grenade
	//

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;


	//
	/// Default Weapon
	//

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> InstaKillWeaponClass;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	UPROPERTY()
	class AInstaKillGameMode* InstaKillGameMode;

public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }	
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* Getbuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetlagCompensation() const { return LagCompensation; }
	ETeam GetTeam();

};
