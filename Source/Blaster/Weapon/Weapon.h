// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX"),

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponStateChanged, EWeaponState, NewState);


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);

	UFUNCTION(BlueprintCallable)
		class AMagazine* EjectMagazine();

	UFUNCTION()
		void SpawnCasing();

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnWeaponStateChanged OnWeaponStateChanged;
	/*
	* Textures for the weapon Crosshairs
	*/
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere)
		class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere)
		class USoundCue* SniperReload;

	bool bDestroyWeapon = false;

	/*
	*** Zoomed FOV while Aiming
	*/

	UPROPERTY(EditAnywhere)
		float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed = 20.f;

	//
	/// Automatic Fire
	//

	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = true;

	//
	/// Enable or disable custom depth
	//

	void EnableCustomDepth(bool bEnable);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int32 Ammo;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState WeaponState;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* AreaSphere;

	UFUNCTION()
		void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		class UAnimationAsset* MagazineAnimation;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACasing> CasingClass;

	UFUNCTION()
		void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere)
		int32 MagCapacity;

	UPROPERTY()
		class ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
		class ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UTexture2D* AmmoTypeIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UTexture2D* WeaponTypeIcon;

	UPROPERTY(EditAnywhere, Category = MagazineEject)
		TSubclassOf<class AMagazine> MagazineClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	FTransform MagazineTransform;

	UPROPERTY(EditAnywhere, Category = MagazineEject)
		float MagSpawnDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = MagazineEject)
		FRotator MagazineSpawnRotation;

	class UBoxComponent* CollisionBox;

	bool bHasStateChanged;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
		float DestroyTime;

	void StartDestroyTimer();

	void DestroyActor();

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetmagCapacity() const { return MagCapacity; }
	FORCEINLINE UAnimationAsset* GetMagazineAnimation() const { return MagazineAnimation; }

	//testing
	FORCEINLINE UTexture2D* GetWeaponTypeIcon() const { return WeaponTypeIcon; }
	FORCEINLINE UTexture2D* GetAmmoTypeIcon() const { return AmmoTypeIcon; }
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
};