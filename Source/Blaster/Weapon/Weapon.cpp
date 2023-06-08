// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Magazine.h"
#include "Engine/SKeletalMeshSocket.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/BoxComponent.h"
#include "Blaster/Blaster.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/BlasterComponents/CombatComponent.h"


AWeapon::AWeapon()
//ClipBoneName(TEXT("Clip_Bone"))
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_WHITE);
	WeaponMesh->MarkRenderStateDirty();

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	if (WeaponType == EWeaponType::EWT_RocketLauncher || WeaponType == EWeaponType::EWT_GrenadeLauncher)
	{
		bUseServerSideRewindDefault = false;
		bUseServerSideRewind = false;
	}
	/*
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(WeaponMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);*/
}


void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (AreaSphere)
	{
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	else
	{

	}

	if (WeaponMesh)
	{
		EnableCustomDepth(true);
	}
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::PollInit()
{
	if (!bHasSetController && HasAuthority() && BlasterOwnerCharacter && BlasterOwnerCharacter->Controller)
	{
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if (BlasterOwnerController && bUseServerSideRewindDefault && !BlasterOwnerController->HighPingDelegate.IsBound())
		{
			BlasterOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
			bHasSetController = true;
		}
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetHUDAmmo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter)
	{
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if (BlasterOwnerController)
		{
			BlasterOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else if (BlasterOwnerCharacter && BlasterOwnerCharacter->IsLocallyControlled())
	{
		++Sequence;
	}
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority()) return;
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter && BlasterOwnerCharacter->GetCombat() && IsFull())
	{
		BlasterOwnerCharacter->GetCombat()->JumpToShotgunEnd();
	}
	SetHUDAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(Owner) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && BlasterOwnerCharacter->GetEquippedWeapon() && BlasterOwnerCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
		}
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SMG)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		}
		GetWorldTimerManager().ClearTimer(DestroyTimer);

		EnableCustomDepth(false);

		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && bUseServerSideRewindDefault && !BlasterOwnerCharacter->IsLocallyControlled() && !BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}

		// Call the OnWeaponStateChanged delegate
		if (!bHasStateChanged)
		{
			OnWeaponStateChanged.Broadcast(State);
			bHasStateChanged = true;
		}
		break;

	case EWeaponState::EWS_EquippedSecondary:

		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SMG)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		}
		GetWorldTimerManager().ClearTimer(DestroyTimer);

		EnableCustomDepth(false);

		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}

		// Call the OnWeaponStateChanged delegate
		if (!bHasStateChanged)
		{
			OnWeaponStateChanged.Broadcast(State);
			bHasStateChanged = true;
		}

		break;

	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->MarkRenderStateDirty();
		StartDestroyTimer();
		EnableCustomDepth(true);

		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}

		break;
	}
}

void AWeapon::DestroyActor()
{
	if (AreaSphere)
	{
		AreaSphere->OnComponentBeginOverlap.RemoveDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.RemoveDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	Destroy();
}

void AWeapon::StartDestroyTimer()
{
	// Create a timer that calls DestroyActor after 1 second
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AWeapon::DestroyActor, DestroyTime, false);
}

void AWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SMG)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		EnableCustomDepth(false);

		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && bUseServerSideRewindDefault && !BlasterOwnerCharacter->IsLocallyControlled() && !BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}
		break;

	case EWeaponState::EWS_EquippedSecondary:

		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SMG)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		}
		EnableCustomDepth(false);
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}
		break;

	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);

		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}
		break;
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
		UUserWidget* UserWidget = PickupWidget->GetUserWidgetObject();

		if (UserWidget)
		{
			UTextBlock* AmmoText = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("AmmoText")));
			UTextBlock* WeaponTypeText = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("WeaponType")));
			UImage* ImageWidget = Cast<UImage>(UserWidget->GetWidgetFromName(TEXT("AmmoTypeIcon")));

			if (AmmoText)
			{
				AmmoText->SetText(FText::FromString(FString::FromInt(Ammo)));
			}

			if (AmmoTypeIcon)
			{
				ImageWidget->SetBrushFromTexture(AmmoTypeIcon);
			}

			// Get the WeaponType block from the widget
			if (WeaponTypeText)
			{
				// Update the text to display the weapon type
				switch (WeaponType)
				{
				case EWeaponType::EWT_AssaultRifle:
					WeaponTypeText->SetText(FText::FromString(TEXT("Assault Rifle")));
					break;

				case EWeaponType::EWT_RocketLauncher:
					WeaponTypeText->SetText(FText::FromString(TEXT("Rocket Launcher")));
					break;

				case EWeaponType::EWT_Pistol:
					WeaponTypeText->SetText(FText::FromString(TEXT("Pistol")));
					break;

				case EWeaponType::EWT_SMG:
					WeaponTypeText->SetText(FText::FromString(TEXT("SMG")));
					break;

				case EWeaponType::EWT_M4AZ:
					WeaponTypeText->SetText(FText::FromString(TEXT("M4AZapper")));
					break;

				case EWeaponType::EWT_Shotgun:
					WeaponTypeText->SetText(FText::FromString(TEXT("Shotgun")));
					break;

				case EWeaponType::EWT_SniperRifle:
					WeaponTypeText->SetText(FText::FromString(TEXT("Sniper Rifle")));
					break;

				case EWeaponType::EWT_GrenadeLauncher:
					WeaponTypeText->SetText(FText::FromString(TEXT("Grenade Launcher")));
					break;

				default:
					WeaponTypeText->SetText(FText::FromString(TEXT("Unknown")));
					break;
				}
			}
		}
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass && WeaponType != EWeaponType::EWT_SniperRifle && WeaponType != EWeaponType::EWT_Shotgun)
	{
		SpawnCasing();
	}
	else if (CasingClass && WeaponType == EWeaponType::EWT_SniperRifle)
	{
		FTimerDelegate TimerCallback;
		FTimerHandle TimerHandle;
		TimerCallback.BindUFunction(this, FName("SpawnCasing"));
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, 1.2f, false);
	}
	else if (CasingClass && WeaponType == EWeaponType::EWT_Shotgun)
	{
		FTimerDelegate TimerCallback;
		FTimerHandle TimerHandle;
		TimerCallback.BindUFunction(this, FName("SpawnCasing"));
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, .5f, false);
	}
	SpendRound();
}


FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return FVector();

	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector TraceStart = SocketTransform.GetLocation();

	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	UCombatComponent* CombatComponent = GetOwner()->FindComponentByClass<UCombatComponent>();

	if (CombatComponent && CombatComponent->IsAiming())
	{
		FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadiusWhenAimed);
		FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
	}
	else
	{
		FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
	}

	/*
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Cyan,
		true
	);*/

}

void AWeapon::SpawnCasing()
{
	const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
	if (AmmoEjectSocket)
	{
		FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<ACasing>(
				CasingClass,
				SocketTransform.GetLocation(),
				SocketTransform.GetRotation().Rotator()
				);
		}
	}
}

void AWeapon::Dropped()
{
	if (WeaponState == EWeaponState::EWS_EquippedSecondary)
	{
		SetWeaponState(EWeaponState::EWS_Dropped);
		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		WeaponMesh->DetachFromComponent(DetachRules);

		SetOwner(nullptr);
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		SetWeaponState(EWeaponState::EWS_Dropped);
		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		WeaponMesh->DetachFromComponent(DetachRules);

		if (BlasterOwnerCharacter)
		{
			FVector Impulse = BlasterOwnerCharacter->GetActorForwardVector() * 500.f; //@TODO, this line was crashing the engine on quit game. check if it's still crashing shit.
			WeaponMesh->AddImpulse(Impulse, NAME_None, true);
		}

		SetOwner(nullptr);
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
}


bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

AMagazine* AWeapon::EjectMagazine()
{

	//UE_LOG(LogTemp, Warning, TEXT("Ejecting magazine..."));

	const USkeletalMeshSocket* MagazineEjectSocket = WeaponMesh->GetSocketByName(FName("MagazineEject"));
	if (MagazineEjectSocket)
	{
		FTransform SocketTransform = MagazineEjectSocket->GetSocketTransform(WeaponMesh);

		UWorld* World = GetWorld();
		if (World)
		{
			AMagazine* SpawnedMagazine = World->SpawnActor<AMagazine>(
				MagazineClass,
				SocketTransform.GetLocation(),
				SocketTransform.GetRotation().Rotator()
				);

			return SpawnedMagazine;
		}
	}
	return nullptr;
}