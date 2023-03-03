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
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
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
	
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	if (WeaponMesh)
	{
		EnableCustomDepth(false);
	}
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
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
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
	if (WeaponMesh && BlasterCharacter->IsLocallyControlled())
	{
		EnableCustomDepth(true);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
	if (WeaponMesh && BlasterCharacter->IsLocallyControlled())
	{
		EnableCustomDepth(false);
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
	Ammo = FMath::Clamp(Ammo -1, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::OnRep_Ammo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter && BlasterOwnerCharacter->GetCombat() && IsFull())
	{
		BlasterOwnerCharacter->GetCombat()->JumpToShotgunEnd();
	}
	//--Ammo;
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
		SetHUDAmmo();
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
		EnableCustomDepth(false);
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
		EnableCustomDepth(false);
		break;
	}
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
		EnableCustomDepth(false);
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
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
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

	UE_LOG(LogTemp, Warning, TEXT("Ejecting magazine..."));

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