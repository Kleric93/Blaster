// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "Blaster/BlasterUserSettings.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blaster/BlasterEnhancedInputComponent.h"
#include "Blaster/Input/BlasterGameplayTags.h"
#include "Components/InputComponent.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/Blaster.h"
#include "BlasterAnimInstance.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/GameMode/InstaKillGameMode.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Weapon/Magazine.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/Pickups/TeamsFlag.h"
#include "Blaster/PlayerStart/TeamPlayerStart.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SphereComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/BillboardComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerMappableInputConfig.h"
#include "Blaster/HUD/OverheadWidget.h"




ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverheadBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("OverheadBoom"));
	OverheadBoom->SetupAttachment(GetCapsuleComponent());
	OverheadBoom->TargetArmLength = 600.f;
	OverheadBoom->bUsePawnControlRotation = true;
	OverheadBoom->bDoCollisionTest = false;

	OverheadWidgetBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Overhead Widget COllision Box"));
	OverheadWidgetBox->SetupAttachment(GetCapsuleComponent());
	OverheadWidgetBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverheadWidgetBox->SetCollisionObjectType(ECC_GameTraceChannel3);
	OverheadWidgetBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverheadWidgetBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);


	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(GetMesh());
	OverheadWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	OverheadWidgetComponent->SetDrawSize(FVector2D(5000.f, 5000.f));
	OverheadWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	OverheadWidgetComponent->SetVisibility(false);
	OverheadWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));

	//OverheadBuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Overhead Buff Component"));
	//OverheadBuffComponent->SetupAttachment(GetRootComponent());
	//OverheadBuffComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));

	AimAssistSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AimAssistSphere"));
	AimAssistSphere->SetupAttachment(GetMesh());
	AimAssistSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AimAssistSphere->SetCollisionObjectType(ECC_EngineTraceChannel4);
	AimAssistSphere->SetCollisionResponseToChannel(ECC_EngineTraceChannel4, ECR_Block);

	HeadIconSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("HeadIcon"));
	HeadIconSpriteComponent->SetupAttachment(GetCapsuleComponent());
	HeadIconSpriteComponent->SetRelativeLocation(HeadIconLocation);

	MinimapCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCamera"));
	MinimapCamera->SetupAttachment(GetCapsuleComponent());

	MinimapRenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("MinimapRenderTarget"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));


	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);


	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//
	/// Hit Boxes for Server-side Rewind
	//

	#pragma region HitBoxes Construction

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);


	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blankett = CreateDefaultSubobject<UBoxComponent>(TEXT("blankett"));
	blankett->SetupAttachment(GetMesh(), FName("blanket_l"));
	HitCollisionBoxes.Add(FName("blankett"), blankett);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

#pragma endregion

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			OnPlayerStateInitialized();

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(BlasterPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
	if (BlasterPlayerController == nullptr)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
		if (BlasterPlayerController && Combat && StartingWeapon)
		{
			BlasterPlayerController->SetHUDWeaponType(StartingWeapon->GetWeaponType());
			BlasterPlayerController->SetHUDWeaponAmmo(StartingWeapon->GetAmmo());
			Combat->UpdateCarriedAmmo();
		}
	}
}

void ABlasterCharacter::UpdateMinimapRenderTarget()
{
	if (MinimapRenderTarget && IsLocallyControlled())
	{
		MinimapCamera->TextureTarget = MinimapRenderTarget;
	}
}

void ABlasterCharacter::UpdateHUDFlag()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	TArray<AActor*> FoundFlags;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamsFlag::StaticClass(), FoundFlags);
	for (AActor* Actor : FoundFlags)
	{
		ATeamsFlag* TheFlag = Cast<ATeamsFlag>(Actor);

		if (BlasterPlayerController && Combat && TheFlag && TheFlag->ActorHasTag("BlueFlagTag"))
		{
			BlasterPlayerController->SetHUDBlueFlagState(TheFlag, TheFlag->GetFlagState());
		}
		if (BlasterPlayerController && Combat && TheFlag && TheFlag->ActorHasTag("RedFlagTag"))
		{
			BlasterPlayerController->SetHUDRedFlagState(TheFlag, TheFlag->GetFlagState());
		}
	}
}

EPhysicalSurface ABlasterCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation()};
	const FVector End{ Start + FVector(0.f, 0.f, -400.f) };
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		QueryParams);
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void ABlasterCharacter::GetWidgetVisibility()
{

}



void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

ETeam ABlasterCharacter::GetTeam()
{
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterPlayerState == nullptr) return ETeam::ET_NoTeam;
	return BlasterPlayerState->GetTeam();
}

void ABlasterCharacter::Elim(bool bPlayerLeftGame)
{		
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		if (Combat->EquippedFlag)
		{
			Combat->EquippedFlag->ServerDetachfromBackpack();
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				DroppedFlag
			);
		}
		/*
		else
		{
			return;  // @TODO Test if it crashes without this line. (it shouldn't)
		}*/
	}
	ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(Controller);
	if (PC)
	{
		PC->EventBorderDeath();
	}
	MulticastElim(bPlayerLeftGame);
}

void ABlasterCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();
	RespawnSentence();

	// Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	bDisableGameplay = true;

	// Disable character movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetAimAssistSphereComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn Elim Bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}

	bool bHideSniperScope = IsLocallyControlled() &&
		Combat &&
		Combat->bAiming &&
		Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;

	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}

	bool bHideM4Scope = IsLocallyControlled() &&
		Combat &&
		Combat->bAiming &&
		Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_M4AZ;

	if (bHideM4Scope)
	{
		ShowM4ScopeWidget(false);
	}

	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}

	if (OverheadBuffComponent)
	{
		OverheadBuffComponent->DestroyComponent();
	}
	
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlasterCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void ABlasterCharacter::ElimTimerFinished()
{
	BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
	}
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::OnPlayerStateInitialized()
{
	BlasterPlayerState->AddToScore(0.f);
	BlasterPlayerState->AddToDefeats(0);

	SetTeamColor(BlasterPlayerState->GetTeam());
	SetSpawnPoint();
	SetHeadIcon();

	ABlasterPlayerState* PlayerJoining = Cast<ABlasterPlayerState>(this->GetPlayerState());
	if (PlayerJoining == nullptr) return;
	ABlasterGameState* BlasterGameState = GetWorld()->GetGameState<ABlasterGameState>();
	if (BlasterGameState && !bHasAlreadyJoined)
	{
		BlasterGameState->Multicast_AddPlayerJoined(PlayerJoining, PlayerJoining->GetPlayerName());
		bHasAlreadyJoined = true;
	}

	if (OverheadWidgetClass)
	{
		UOverheadWidget* OverheadWidget = Cast<UOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject());
		if (!OverheadWidget)
		{
			OverheadWidget = CreateWidget<UOverheadWidget>(GetWorld(), OverheadWidgetClass);
			if (OverheadWidget)
			{
				OverheadWidgetComponent->SetWidget(OverheadWidget);
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Failed to Create Overhead Widget"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Widget already exists"));
		}
		OverheadWidget->ShowPlayerName(this);
		OverheadWidget->ChangeOWColor(GetTeam());
	}

}

void ABlasterCharacter::SetSpawnPoint()
{
	if (HasAuthority() && BlasterPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == BlasterPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		if (TeamPlayerStarts.Num() > 0)
		{
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			SetActorLocationAndRotation(
				ChosenPlayerStart->GetActorLocation(),
				ChosenPlayerStart->GetActorRotation()
			);
		}
	}
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::RespawnSentence()
{
	if (bLeftGame == true) return;
	FTransform SpawnTransform = GetActorTransform();
	UGameplayStatics::SpawnSoundAtLocation(this, RespawnSentences, SpawnTransform.GetLocation());	
}

void ABlasterCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void ABlasterCharacter::MulticastLostTheLead_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void ABlasterCharacter::SetTeamColor(ETeam Team)
{
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_RedTeam:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = OriginalDissolveMatInst;
		break;
	case ETeam::ET_MAX:
		break;
	default:
		break;
	}
}

void ABlasterCharacter::SetHeadIcon()
{
	if (GetTeam() == ETeam::ET_NoTeam)
	{
		if (HeadIconSpriteComponent && DefaultPlayerIcon)
		{
			HeadIconSpriteComponent->SetSprite(DefaultPlayerIcon);
		}
	}
	if (GetTeam() == ETeam::ET_RedTeam)
	{
		if (HeadIconSpriteComponent && DefaultPlayerIcon)
		{
			HeadIconSpriteComponent->SetSprite(RedTeamPlayerIcon);
		}
	}
	if (GetTeam() == ETeam::ET_BlueTeam)
	{
		if (HeadIconSpriteComponent && DefaultPlayerIcon)
		{
			HeadIconSpriteComponent->SetSprite(BlueTeamPlayerIcon);
		}
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Settings == nullptr)
	{
		Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
	}


	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}

	if (MinimapRenderTarget)
	{
		if (IsLocallyControlled())
		{
			MinimapCamera->TextureTarget = MinimapRenderTarget;
		}
	}
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCharacterAndWeaponsIfScopingOrCameraClose();
	PollInit();
	UpdateHUDFlag();

	/*
	if (CurrentBuffComponent.IsValid())
	{
		FVector Location = CurrentBuffComponent->GetComponentLocation();
		UE_LOG(LogTemp, Warning, TEXT("CurrentBuffComponent is valid. Location: %s"), *Location.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentBuffComponent is NULL."));
	}*/
}

#pragma region EnhancedInput

void ABlasterCharacter::Movement(const FInputActionValue& Value)
{
	const FVector2D DirectionValue = Value.Get<FVector2D>();
	if (bDisableGameplay) return;

	if (Controller)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DirectionX(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		const FVector DirectionY(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		AddMovementInput(DirectionX, DirectionValue.X);
		AddMovementInput(DirectionY, DirectionValue.Y);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	LastLookInput = Value.Get<FVector2D>();

	if (Controller && Settings && Combat && Combat->EquippedWeapon)
	{
		bool bIsLongRange =
			Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_M4AZ ||
			Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;

		if (!Combat->IsAiming())
		{
			AddControllerYawInput(LastLookInput.X * Settings->GetBaseTurnRate());
			AddControllerPitchInput(LastLookInput.Y * Settings->GetBaseTurnRate());
		}
		else if (Combat->IsAiming() && !bIsLongRange)
		{
			float DecreasedRotationSpeed = Settings->GetBaseTurnRate() / Settings->GetAimingTurnRateForShortZoom();

			AddControllerYawInput(LastLookInput.X * DecreasedRotationSpeed);
			AddControllerPitchInput(LastLookInput.Y * DecreasedRotationSpeed);
		}
		else if (Combat->IsAiming() && bIsLongRange)
		{
			float DecreasedRotationSpeed = Settings->GetBaseTurnRate() / Settings->GetAimingTurnRateForLongZoom();

			AddControllerYawInput(LastLookInput.X * DecreasedRotationSpeed);
			AddControllerPitchInput(LastLookInput.Y * DecreasedRotationSpeed);
		}
		
	}
	else
	{
		AddControllerYawInput(LastLookInput.X);
		AddControllerPitchInput(LastLookInput.Y);
	}
}

// no inputactionvalue needed, cause it's a Super::
void ABlasterCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
		if (Combat->bIsProne)
		{
			Combat->StopProne();
		}
	}
	else
	{
		if (Combat->bIsProne)
		{
			Combat->StopProne();
		}
		Super::Jump();
	}
}

// no inputactionvalue needed, cause no value in signature
void ABlasterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		ServerEquipButtonPressed();
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
	}
}

// no inputactionvalue needed, cause no value in signature
void ABlasterCharacter::SwapButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerSwapButtonPressed();

		bool bSwap = Combat->ShouldSwapWeapons() &&
			!HasAuthority() &&
			Combat->CombatState == ECombatState::ECS_Unoccupied &&
			OverlappingWeapon == nullptr;

		if (bSwap)
		{
			PlaySwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}

void ABlasterCharacter::ServerSwapButtonPressed_Implementation()
{
	if (Combat && Combat->ShouldSwapWeapons())
	{
		Combat->SwapWeapon();
	}
}

// no inputactionvalue needed, cause no value in signature
void ABlasterCharacter::CrouchButtonPressed()
{
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		Combat->StopProne();
		UnCrouch();
	}
	if(Speed <= 400.f || Combat->bIsProne)
	{
		Combat->bIsProne = false;
		Crouch();
	}
	if (Speed > 400.f && !bIsCrouched)
	{
		if (Combat->bIsSliding == true || bIsInAir)
		{
			Combat->StopSliding();
		}
		else if(!Combat->bAiming)
		{
			Combat->StartSliding();
		}
	}
}

void ABlasterCharacter::ProneButtonPressed()
{
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	bool bIsSliding = Combat->bIsSliding;

	if (bDisableGameplay) return;
	if (Combat->bIsProne == false)
	{
		Combat->StopSliding();
	}
	if (Combat->bIsProne == true)
	{
		Combat->StopProne();
	}
	else if (Speed >= 0 || bIsInAir || bIsCrouched || bIsSliding)
	{
		UnCrouch();
		Combat->StartProne();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void ABlasterCharacter::Aim(const FInputActionValue& Value)
{
	bool IsAiming = Value.Get<bool>();

	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(IsAiming);
	}
}

void ABlasterCharacter::Fire(const FInputActionValue& Value)
{
	bool IsFiring = Value.Get<bool>();
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(IsFiring);
	}
}

// no inputactionvalue needed, cause no value in signature
void ABlasterCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->Reload();
	}
}

// no inputactionvalue needed, cause no value in signature
void ABlasterCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

#pragma endregion EnhancedInput

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	UBlasterEnhancedInputComponent* EnhancedInputComponent = Cast<UBlasterEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInputComponent);

	const FBlasterGameplayTags& GameplayTags = FBlasterGameplayTags::Get();
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ABlasterCharacter::Movement);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Triggered, this, &ABlasterCharacter::Jump);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Equip, ETriggerEvent::Triggered, this, &ABlasterCharacter::EquipButtonPressed);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Swap, ETriggerEvent::Triggered, this, &ABlasterCharacter::SwapButtonPressed);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ABlasterCharacter::CrouchButtonPressed);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Prone, ETriggerEvent::Triggered, this, &ABlasterCharacter::ProneButtonPressed);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Aim, ETriggerEvent::Triggered, this, &ABlasterCharacter::Aim);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Fire, ETriggerEvent::Triggered, this, &ABlasterCharacter::Fire);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Reload, ETriggerEvent::Triggered, this, &ABlasterCharacter::ReloadButtonPressed);
	EnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Grenade, ETriggerEvent::Triggered, this, &ABlasterCharacter::GrenadeButtonPressed);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed, 
			GetCharacterMovement()->MaxWalkSpeedCrouched
		);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		if (Combat->EquippedWeapon->GetWeaponType() != EWeaponType::EWT_SniperRifle)
		{
			SectionName = bAiming ? FName("RifleIronSights") : FName("RifleHip");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
		else
		{
			SectionName = bAiming ? FName("SniperFireIronsights") : FName("SniperFireIronsights");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
		
	}
}

void ABlasterCharacter::ARMagazineAnimation()
{
	UAnimationAsset* MagAnim = Combat->EquippedWeapon->GetMagazineAnimation();
	Combat->EquippedWeapon->GetWeaponMesh()->PlayAnimation(MagAnim, false);
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// Create a weak reference to the EquippedWeapon object
	TWeakObjectPtr<AWeapon> WeakWeapon = Combat->EquippedWeapon;

	FTimerDelegate TimerCallback;
	FTimerHandle TimerHandle;
	TimerCallback.BindLambda([WeakWeapon]()
		{
			// Check if the EquippedWeapon object is still valid before calling its functions
			if (WeakWeapon.IsValid())
			{
				WeakWeapon->EjectMagazine();
			}
		});

	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			ARMagazineAnimation();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, 0.2f, false);
			break;

		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rocket");
			ARMagazineAnimation();
			break;

		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			ARMagazineAnimation();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, 0.2f, false);
			break;

		case EWeaponType::EWT_SMG:
			SectionName = FName("Rifle");
			ARMagazineAnimation();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, 0.2f, false);
			break;

		case EWeaponType::EWT_M4AZ:
			SectionName = FName("M4AZ");
			ARMagazineAnimation();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, 0.4f, false);
			break;

		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			//ARMagazineAnimation();
			break;

		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Sniper");
			//ARMagazineAnimation();
			break;

		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			//ARMagazineAnimation();
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
		//GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void ABlasterCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr || Combat->bIsSliding || Combat->bIsProne) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage && Combat->CombatState != ECombatState::ECS_Reloading)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	if (bElimmed || BlasterGameMode == nullptr) return;
	Damage = BlasterGameMode->CalculateDamage(InstigatorController, Controller, Damage);

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	UpdateHUDHealth();
	UpdateHUDShield();
	if (Combat->CombatState == ECombatState::ECS_Unoccupied)
	{
		PlayHitReactMontage();
	}

	if (Health == 0.f)
	{
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}

	ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(this->GetController());
	if (PC)
	{
		PC->EventBorderDamage();
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			DamageReceivedSound);
		ShowDamageIndicator(DamagedActor, DamageCauser);
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // Running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled()) //this is to fix the client looking like is aiming up all the time from the server's view
	{
		// map pitch from the range [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;

	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}


void ABlasterCharacter::HideCharacterAndWeaponsIfScopingOrCameraClose()
{
	if (!IsLocallyControlled()) return;
	if (Combat == nullptr || this == nullptr || FollowCamera == nullptr || Combat->EquippedWeapon == nullptr || Combat->EquippedWeapon == nullptr) return;

	bool bSouldHideChar = ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold) || 
		(Combat && Combat->bAiming && GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_M4AZ) ||
		(Combat && Combat->bAiming && GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_SniperRifle);

	if (bSouldHideChar)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;

		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}

	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
		BlasterPlayerController->SetHUDWeaponType(Combat->EquippedWeapon->GetWeaponType());
	}
}

void ABlasterCharacter::SpawnDefaultWeapon()
{
	InstaKillGameMode = Cast<AInstaKillGameMode>(UGameplayStatics::GetGameMode(this));
	if (!InstaKillGameMode)
	{
		BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	}

	UWorld* World = GetWorld();
	if (InstaKillGameMode && World && !bElimmed && InstaKillWeaponClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current game mode: InstaKillGameMode"));

		StartingWeapon = World->SpawnActor<AWeapon>(InstaKillWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
	else if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current game mode: BlasterGameMode"));

		StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}


void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
	
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);

	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return nullptr;  //@TODO sometimes this crashes the engine, investigate further;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;

}

void ABlasterCharacter::AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, ULocalPlayer* LocalPlayer)
{
	for (const FEnhancedActionKeyMapping& Mapping : Config->GetPlayerMappableKeys())
	{
		// Make sure that the mapping has a valid name, its possible to have an empty name
		// if someone has marked a mapping as "Player Mappable" but deleted the default field value
		if (Mapping.PlayerMappableOptions.Name != NAME_None)
		{
			CustomKeyboardConfig.Add(Mapping.PlayerMappableOptions.Name, Mapping.Key);
		}
	}

	if (FKey* ExistingMapping = CustomKeyboardConfig.Find(MappingName))
	{
		// Change the key to the new one
		CustomKeyboardConfig[MappingName] = NewKey;
	}
	else
	{
		CustomKeyboardConfig.Add(MappingName, NewKey);
	}

	// Tell the enhanced input subsystem for this local player that we should remap some input! Woo
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->AddPlayerMappedKey(MappingName, NewKey);
	}
}

void ABlasterCharacter::ResetKeybindingToDefault(const FName MappingName, ULocalPlayer* LocalPlayer)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->RemovePlayerMappedKey(MappingName);
	}
}

void ABlasterCharacter::ResetKeybindingsToDefault(ULocalPlayer* LocalPlayer)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->RemoveAllPlayerMappedKeys();
	}
}

void ABlasterCharacter::SpawnOverheadBuff(UNiagaraSystem* BuffType)
{
	if (OverheadBuffComponent == nullptr)
	{
		OverheadBuffComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BuffType,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (OverheadBuffComponent)
	{
		OverheadBuffComponent->SetAsset(BuffType);
		OverheadBuffComponent->Activate();
	}
	GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void ABlasterCharacter::DeactivateOverheadBuffComponent()
{
	if (OverheadBuffComponent)
	{
		OverheadBuffComponent->DestroyComponent();
	}
	GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

/*
UNiagaraComponent* ABlasterCharacter::SpawnOverheadBuff(UNiagaraSystem* BuffType, float BuffTime)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnOverheadBuff called."));

	if (BuffType)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffType: %s"), *BuffType->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffType is null."));
	}

	if (CurrentBuffComponent.IsValid())
	{
		GetWorldTimerManager().ClearTimer(BuffTimerHandle);
		CurrentBuffComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		CurrentBuffComponent->DestroyComponent();
		CurrentBuffComponent = nullptr;
	}

	UNiagaraComponent* NewBuffComponent = NewObject<UNiagaraComponent>(this->GetRootComponent());

	if (NewBuffComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewBuffComponent created: %s"), *NewBuffComponent->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NewBuffComponent is null."));
		return nullptr;
	}

	NewBuffComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	NewBuffComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	if (NewBuffComponent->GetAttachParent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Buff's parent after attaching: %s"), *NewBuffComponent->GetAttachParent()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Buff's parent is null after attaching."));
	}

	if (GetRootComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Root Component: %s"), *GetRootComponent()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Root Component is null."));
	}

	NewBuffComponent->SetAsset(BuffType);
	NewBuffComponent->RegisterComponent();
	NewBuffComponent->Activate();

	if (GetWorld())
	{
		CurrentBuffComponent = NewBuffComponent;
		GetWorldTimerManager().SetTimer(
			BuffTimerHandle,
			[this]()
			{
				if (BuffTimerHandle.IsValid() && CurrentBuffComponent.IsValid())
				{
					CurrentBuffComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					CurrentBuffComponent->DestroyComponent();
					UE_LOG(LogTemp, Warning, TEXT("Buff timer expired and CurrentBuffComponent destroyed."));
					CurrentBuffComponent = nullptr;
				}
			},
			BuffTime,
				false);
	}


	return NewBuffComponent;
}


void ABlasterCharacter::ClearBuff()
{
	UE_LOG(LogTemp, Warning, TEXT("ClearBuff called."));
	if (BuffTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(BuffTimerHandle);
		if (CurrentBuffComponent.IsValid())
		{
			CurrentBuffComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			UE_LOG(LogTemp, Warning, TEXT("About to destroy CurrentBuffComponent."));
			CurrentBuffComponent->DestroyComponent();
			CurrentBuffComponent = nullptr;
		}
		BuffTimerHandle.Invalidate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffTimerHandle is not valid."));
	}
}*/



void ABlasterCharacter::ShowDamageIndicator(AActor* DamagedActor, AActor* DamageCauser)
{
	ABlasterCharacter* DamagedCharacter = Cast<ABlasterCharacter>(DamagedActor);
	if (DamagedCharacter && DamageCauser)
	{
		FRotator Direction = UKismetMathLibrary::FindLookAtRotation(DamagedActor->GetActorLocation(), DamageCauser->GetActorLocation());
		FRotator PlayerRot = DamagedCharacter->GetControlRotation();
		float Angle = Direction.Yaw - PlayerRot.Yaw;

		FVector CameraLocation = DamagedCharacter->GetFollowCamera()->GetComponentLocation();
		//DamagedCharacter->PlayCameraShake(HitCameraShake, CameraLocation);
		ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(DamagedCharacter->Controller);
		if (PlayerController)
		{
			PlayerController->SetHUDDamageIndicator(Angle);
		}
	}
	MulticastShowDamageIndicator(DamagedActor, DamageCauser);
}

void ABlasterCharacter::MulticastShowDamageIndicator_Implementation(AActor* DamagedActor, AActor* DamageCauser)
{
	ABlasterCharacter* DamagedCharacter = Cast<ABlasterCharacter>(DamagedActor);
	if (DamagedCharacter && DamageCauser)
	{
		FRotator Direction = UKismetMathLibrary::FindLookAtRotation(DamagedActor->GetActorLocation(), DamageCauser->GetActorLocation());
		FRotator PlayerRot = DamagedCharacter->GetControlRotation();
		float Angle = Direction.Yaw - PlayerRot.Yaw;

		FVector CameraLocation = DamagedCharacter->GetFollowCamera()->GetComponentLocation();
		//DamagedCharacter->PlayCameraShake(HitCameraShake, CameraLocation);
		ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(DamagedCharacter->Controller);
		if (PlayerController)
		{
			PlayerController->SetHUDDamageIndicator(Angle);
		}
	}
}