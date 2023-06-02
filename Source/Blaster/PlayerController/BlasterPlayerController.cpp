// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Components/Image.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/HUD/ReturnToMainMenu.h"
#include "Blaster/HUD/VotingSyastem.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/Announcement.h"
#include "Blaster/Pickups/TeamsFlag.h"
#include "Blaster/Pickups/FlagTypes.h"
#include "Blaster/BlasterTypes/Team.h"
#include "Blaster/HUD/ChatSystemOverlay.h"
#include "Components/EditableText.h"
#include "Blaster/HUD/PlayerStats.h"
#include "Blaster/HUD/ScoresOverview.h"
#include "Blaster/HUD/PlayerStatsLine.h"
#include "Blaster/Pickups/SpeedPickup.h"
#include "Blaster/Pickups/JumpPickup.h"
#include "Blaster/Pickups/BerserkPickup.h"
#include "Blaster/Pickups/PickupSpawnPoint.h"
#include "Blaster/HUD/SettingsMenu.h"
#include "EngineUtils.h"
#include "Blaster/BlasterUserSettings.h"
#include "Blaster/HUD/TeamChoice.h"
#include "Blaster/BlasterComponents/BuffComponent.h"


ABlasterPlayerController::ABlasterPlayerController()
{
	bReplicates = true;
}


void ABlasterPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void ABlasterPlayerController::HideTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore &&
		BlasterHUD->CharacterOverlay->BlueTeamScore &&
		BlasterHUD->CharacterOverlay->Spacer &&
		BlasterHUD->CharacterOverlay->TeamScoresText;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->Spacer->SetText(FText());
		BlasterHUD->CharacterOverlay->TeamScoresText->SetText(FText());
	}
}

void ABlasterPlayerController::HideTeamFlagIcons()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BlueFlagState &&
		BlasterHUD->CharacterOverlay->RedFlagState;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->BlueFlagState->SetRenderOpacity(0.f);
		BlasterHUD->CharacterOverlay->RedFlagState->SetRenderOpacity(0.f);
	}
}

void ABlasterPlayerController::InitTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore &&
		BlasterHUD->CharacterOverlay->BlueTeamScore &&
		BlasterHUD->CharacterOverlay->Spacer &&
		BlasterHUD->CharacterOverlay->TeamScoresText;

	if (bHUDValid)
	{
		FString Zero("0");
		FString Spacer("|");
		FString TeamsText("Team Scores");
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(Zero));
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(Zero));
		BlasterHUD->CharacterOverlay->Spacer->SetText(FText::FromString(Spacer));
		BlasterHUD->CharacterOverlay->TeamScoresText->SetText(FText::FromString(TeamsText));
	}
}

void ABlasterPlayerController::InitFlagIcons()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BlueFlagState &&
		BlasterHUD->CharacterOverlay->RedFlagState;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->BlueFlagState->SetRenderOpacity(1.f);
		BlasterHUD->CharacterOverlay->RedFlagState->SetRenderOpacity(1.f);
	}
}

void ABlasterPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), RedScore);
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDRedFlagState(AActor* Flag, EFlagState NewFlagState)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BlueFlagState;

	if (bHUDValid)
	{
		ATeamsFlag* TheFlag = Cast<ATeamsFlag>(Flag);

		TheFlag->OnRedFlagStateChanged.AddDynamic(this, &ABlasterPlayerController::UpdateRedFlagStateInHUD);
		UpdateRedFlagStateInHUD(NewFlagState);
	}
}

void ABlasterPlayerController::UpdateRedFlagStateInHUD(EFlagState NewFlagState)
{
	// Check if BlasterHUD and CharacterOverlay are not null
	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		// Get the BlueFlagState image widget from the HUD
		UImage* RedFlagState = BlasterHUD->CharacterOverlay->RedFlagState;

		// Check if RedFlagState is not null
		if (RedFlagState)
		{
			// Set the image based on the new flag state
			switch (NewFlagState)
			{
			case EFlagState::EFS_Initial:
				RedFlagState->SetBrushFromTexture(RedFlagInitial);
				break;
			case EFlagState::EFS_Equipped:
				RedFlagState->SetBrushFromTexture(RedFlagStolen);
				break;
			case EFlagState::EFS_Dropped:
				RedFlagState->SetBrushFromTexture(RedFlagDropped);
				break;
			default:
				break;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RedFlagState is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BlasterHUD or CharacterOverlay is null"));
	}
}


void ABlasterPlayerController::AddChatBox()
{
	if (!IsLocalPlayerController()) return;
	//UE_LOG(LogTemp, Warning, TEXT("Chatboxadded from !islocalplayercontroller IF"));

	if (ChatSystemOverlayClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Chatboxadded from 1st IF"));

		ChatSystemWidget = ChatSystemWidget == nullptr ? CreateWidget<UChatSystemOverlay>(this, ChatSystemOverlayClass) : ChatSystemWidget;
		if (ChatSystemWidget)
		{
			ChatSystemWidget->AddToViewport();
			ChatSystemWidget->InputTextBox->SetVisibility(ESlateVisibility::Collapsed);
			ChatSystemWidget->InputTextBox->OnTextCommitted.AddDynamic(this, &ABlasterPlayerController::OnTextCommitted);
			//UE_LOG(LogTemp, Warning, TEXT("Chatboxadded from 2nd IF"));

		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("class is null"));

	}

}

void ABlasterPlayerController::ToggleInputChatBox()
{
	if (ChatSystemWidget && ChatSystemWidget->InputTextBox)
	{
		//UE_LOG(LogTemp, Error, TEXT("ToggleInputChatbox"));
		if (ChatSystemWidget->InputTextBox->GetVisibility() == ESlateVisibility::Collapsed)
		{
			ChatSystemWidget->InputTextBox->SetVisibility(ESlateVisibility::Visible);
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(ChatSystemWidget->InputTextBox->TakeWidget());
			SetInputMode(InputMode);
			SetShowMouseCursor(true);
			//UE_LOG(LogTemp, Error, TEXT("ToggleInputChatbox"));

		}
		else
		{
			ChatSystemWidget->InputTextBox->SetVisibility(ESlateVisibility::Collapsed);
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
			SetShowMouseCursor(false);
			//UE_LOG(LogTemp, Error, TEXT("ToggleInputChatbox else"));

		}
	}
}

void ABlasterPlayerController::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter) return;

	PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
	FString PlayerName("");
	if (PlayerState)
	{
		PlayerName = PlayerState->GetPlayerName();
	}
	if (ChatSystemWidget)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Here"));
		if (!Text.IsEmpty())
		{
			ServerSetText(Text.ToString(), PlayerName);
		}
		ChatSystemWidget->InputTextBox->SetText(FText());
		ChatSystemWidget->InputTextBox->SetVisibility(ESlateVisibility::Collapsed);
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
	}
}

void ABlasterPlayerController::ClientSetText_Implementation(const FString& Text, const FString& PlayerName)
{
	PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
	if (ChatSystemWidget && PlayerState)
	{
		if (PlayerName == PlayerState->GetPlayerName())
		{
			ChatSystemWidget->SetChatText(Text, "You");
		}
		else
		{
			ChatSystemWidget->SetChatText(Text, PlayerName);
		}
	}
}

void ABlasterPlayerController::ServerSetText_Implementation(const FString& Text, const FString& PlayerName)
{
	BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
	if (BlasterGameMode)
	{
		BlasterGameMode->SendChat(Text, PlayerName);
	}
}

void ABlasterPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), BlueScore);
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDBlueFlagState(AActor* Flag, EFlagState NewFlagState)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BlueFlagState;

	if (bHUDValid)
	{
		ATeamsFlag* TheFlag = Cast<ATeamsFlag>(Flag);

		TheFlag->OnBlueFlagStateChanged.AddDynamic(this, &ABlasterPlayerController::UpdateBlueFlagStateInHUD);
		UpdateBlueFlagStateInHUD(NewFlagState);
	}
}

void ABlasterPlayerController::UpdateBlueFlagStateInHUD(EFlagState NewFlagState)
{
	// Check if BlasterHUD and CharacterOverlay are initialized
	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		// Get the BlueFlagState image widget from the HUD
		UImage* BlueFlagState = BlasterHUD->CharacterOverlay->BlueFlagState;

		// Check if BlueFlagState is initialized
		if (BlueFlagState)
		{
			// Set the image based on the new flag state
			switch (NewFlagState)
			{
			case EFlagState::EFS_Initial:
				BlueFlagState->SetBrushFromTexture(BlueFlagInitial);
				break;
			case EFlagState::EFS_Equipped:
				BlueFlagState->SetBrushFromTexture(BlueFlagStolen);
				break;
			case EFlagState::EFS_Dropped:
				BlueFlagState->SetBrushFromTexture(BlueFlagDropped);
				break;
			default:
				break;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BlueFlagState is not initialized."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BlasterHUD or CharacterOverlay are not initialized."));
	}
}


void ABlasterPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	ABlasterCharacter* AttackerCharacter = Cast<ABlasterCharacter>(Attacker->GetPawn());
	AWeapon* Weapon = AttackerCharacter->GetEquippedWeapon();
	UTexture2D* WeaponIcon = Weapon->GetWeaponTypeIcon();

	if (Attacker && Victim && Self)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if (BlasterHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				BlasterHUD->AddElimAnnouncement("You", Victim->GetPlayerName(), WeaponIcon);
				return;
			}
			if (Victim == Self && Attacker != Self)
			{
				BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "you", WeaponIcon);
				return;
			}
			if (Attacker == Victim && Attacker == Self)
			{
				BlasterHUD->AddElimAnnouncement("You", "Yourself", WeaponIcon);
				return;
			}
			if (Attacker == Victim && Attacker != Self)
			{
				BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves", WeaponIcon);
				return;
			}
			BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName(), WeaponIcon);
		}
	}
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (Settings == nullptr)
	{
		Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
	}
	if (Settings->GetIsUsingKBM() == true)
	{
		IMCSelector(BlasterMappingContextKBM, BlasterMappingContextController);
	}
	else
	{
		IMCSelector(BlasterMappingContextController, BlasterMappingContextKBM);
	}

	AddChatBox();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckmatchState();



	ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(this->PlayerState);
	if (BlasterPlayerState)
	{
		BlasterPlayerState->RegisterBuffSpawnPoints();
	}
}

void ABlasterPlayerController::IMCSelector(UInputMappingContext* MappingContexttoAdd, UInputMappingContext* MappingContexttoRemove)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContexttoAdd, 0);
		Subsystem->RemoveMappingContext(MappingContexttoRemove);
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
	DOREPLIFETIME(ABlasterPlayerController, bShowTeamScores);
	DOREPLIFETIME(ABlasterPlayerController, bShowFlagIcons);
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
}

void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	highPingRunningTime += DeltaTime;
	if (highPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PlayerState->GetPing() * 4: %d"), PlayerState->GetPing() * 4);
			if (PlayerState->GetPing() * 4 > HighPingThreshold) //ping is compressed; it's actually Ping/4
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		highPingRunningTime = 0.f;
	}
	bool bHighPingAnimationplaying = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation);
	if (bHighPingAnimationplaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void ABlasterPlayerController::ShowReturnToMainMenu()
{
	if (bSettingsMenuOpen) return;
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		if (!bReturnToMainMenuOpen)
		{
			bReturnToMainMenuOpen = true;
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
			bReturnToMainMenuOpen = false;
		}
	}
}


void ABlasterPlayerController::ShowVotingSystem()
{
	if (VotingSystemWidget == nullptr) return;
	if (VotingSystem == nullptr)
	{
		VotingSystem = CreateWidget<UVotingSyastem>(this, VotingSystemWidget);
	}
	if (VotingSystem)
	{
		VotingSystem->MenuSetup();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (this->VotingSystem)
				{
					this->VotingSystem->MenuTeardown();
				}
			}, VotingEndedTimer, false);
	}
}

void ABlasterPlayerController::Server_FFAVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetFFAVotes();
	}
}


void ABlasterPlayerController::Server_FFASMVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetFFASMVotes();
	}
}

void ABlasterPlayerController::Server_TDMVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetTDMVotes();
	}
}

void ABlasterPlayerController::Server_TDMSMVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetTDMSMVotes();
	}
}

void ABlasterPlayerController::Server_CTFVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetCTFVotes();
	}
}

void ABlasterPlayerController::Server_CTFSMVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetCTFSMVotes();
	}
}

void ABlasterPlayerController::Server_InstaKillVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetInstaKillVotes();
	}
}

void ABlasterPlayerController::Server_InstaKillSMVoteCast_Implementation()
{
	ABlasterGameState* GameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetInstaKillSMVotes();
	}
}

void ABlasterPlayerController::Server_RedTeamChosen_Implementation()
{
	OnTeamChosen.Broadcast(this, ETeam::ET_RedTeam);
	GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void ABlasterPlayerController::Server_BlueTeamChosen_Implementation()
{
	OnTeamChosen.Broadcast(this, ETeam::ET_BlueTeam);
	GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void ABlasterPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}

void ABlasterPlayerController::OnRep_ShowFlagIcons()
{
	if (bShowFlagIcons)
	{
		InitFlagIcons();
	}
	else
	{
		HideTeamFlagIcons();
	}
}

// is the ping too high?
void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->HighPingAnimation,
			0.f,
			5);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
		{
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void ABlasterPlayerController::EventBorderDamage_Implementation()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EventBorderImage &&
		BlasterHUD->CharacterOverlay->DamageBorderAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->EventBorderImage->SetColorAndOpacity(FLinearColor::Red);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->DamageBorderAnimation,
			0.f,
			1);
	}
}

void ABlasterPlayerController::EventBorderHeal_Implementation()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EventBorderImage &&
		BlasterHUD->CharacterOverlay->EventBorderAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->EventBorderImage->SetColorAndOpacity(FLinearColor::Green);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->EventBorderAnimation,
			0.f,
			1);
	}
}

void ABlasterPlayerController::EventBorderShield_Implementation()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EventBorderImage &&
		BlasterHUD->CharacterOverlay->EventBorderAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->EventBorderImage->SetColorAndOpacity(FLinearColor::Blue);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->EventBorderAnimation,
			0.f,
			1);
	}
}

void ABlasterPlayerController::EventBorderPowerUp_Implementation()
{
	FLinearColor PowerupColor{ 0.0f, 0.6f , 1.0f , 1.0f };
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EventBorderImage &&
		BlasterHUD->CharacterOverlay->EventBorderAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->EventBorderImage->SetColorAndOpacity(PowerupColor);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->EventBorderAnimation,
			0.f,
			1);
	}
}

void ABlasterPlayerController::EventBorderDeath_Implementation()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EventBorderImage &&
		BlasterHUD->CharacterOverlay->DeathBorderAnimation &&
		BlasterHUD->CharacterOverlay->YouDiedText;

	if (bHUDValid)
	{

		BlasterHUD->CharacterOverlay->EventBorderImage->SetColorAndOpacity(FLinearColor::Red);
		BlasterHUD->CharacterOverlay->YouDiedText->SetColorAndOpacity(FLinearColor::Red);
		BlasterHUD->CharacterOverlay->PlayAnimation(
			BlasterHUD->CharacterOverlay->DeathBorderAnimation,
			0.f,
			1);
	}
}

void ABlasterPlayerController::ServerCheckmatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = Settings->GetGameTime();
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		bTMatch = GameMode->bTeamsMatch;
		bCTFMatch = GameMode->bCaptureTheFlagMatch;
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime, bTMatch, bCTFMatch);
	}
}

void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bIsTeamMatch, bool bIsCaptureTheFlagMatch)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	bTMatch = bIsTeamMatch;
	bCTFMatch = bIsCaptureTheFlagMatch;
	OnMatchStateSet(MatchState);

	if (BlasterHUD)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			BlasterHUD->AddAnnouncement();

			if (bTMatch || bCTFMatch)
			{
				TeamChoiceWidget = CreateWidget<UTeamChoice>(GetWorld(), TeamChoiceClass);
				TeamChoiceWidget->WidgetSetup();

				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
					{
						if (this->TeamChoiceWidget)
						{
							this->TeamChoiceWidget->WidgetTeardown();
						}
					}, WarmupTime, false);
			}
		}
			
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		SetHUDShield(BlasterCharacter->GetShield(), BlasterCharacter->GetMaxShield());
		SetHUDCarriedAmmo(HUDCarriedAmmo);
		SetHUDWeaponAmmo(HUDWeaponAmmo);
		SetHUDWeaponType(EWeaponType::EWT_Pistol);
		SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
		BlasterCharacter->UpdateMinimapRenderTarget();
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar &&
		BlasterHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));

		// Set the color of the health bar based on the percentage of health remaining
		if (HealthPercent > 0.5f)
		{
			BlasterHUD->CharacterOverlay->HealthBar->SetFillColorAndOpacity(FLinearColor(0.f, 1.f, 0.f)); // Green
		}
		else if (HealthPercent > 0.2f)
		{
			BlasterHUD->CharacterOverlay->HealthBar->SetFillColorAndOpacity(FLinearColor(1.f, 1.f, 0.f)); // Yellow
		}
		else
		{
			BlasterHUD->CharacterOverlay->HealthBar->SetFillColorAndOpacity(FLinearColor(1.f, 0.f, 0.f)); // Red
		}
	}
	else
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ShieldBar &&
		BlasterHUD->CharacterOverlay->ShieldText;

	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));

		// Set the color of the health bar based on the percentage of health remaining
		if (ShieldPercent > 0.5f)
		{
			BlasterHUD->CharacterOverlay->ShieldBar->SetFillColorAndOpacity(FLinearColor(0.f, 0.f, 1.f));
		}
		else if (ShieldPercent > 0.2f)
		{
			BlasterHUD->CharacterOverlay->ShieldBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.8f)); // Yellow
		}
		else
		{
			BlasterHUD->CharacterOverlay->ShieldBar->SetFillColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.6f)); // Red
		}
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;

	}
}

void ABlasterPlayerController::SetHUDWeaponType(EWeaponType WeaponType)
{

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponType;
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	AWeapon* EquippedWeapon = Cast<AWeapon>(BlasterCharacter->GetEquippedWeapon());

	if (bHUDValid && BlasterCharacter && EquippedWeapon && BlasterHUD)
	{
		UTexture2D* AmmoIcon = EquippedWeapon->GetAmmoTypeIcon();
		UTexture2D* WeaponIcon = EquippedWeapon->GetWeaponTypeIcon();

		UImage* AmmoImageWidget = Cast<UImage>(BlasterHUD->CharacterOverlay->GetWidgetFromName(TEXT("AmmoTypeIcon")));
		UImage* WeaponImageWidget = Cast<UImage>(BlasterHUD->CharacterOverlay->GetWidgetFromName(TEXT("WeaponTypeIcon")));
		UImage* DefaultAmmoWidget = Cast<UImage>(BlasterHUD->CharacterOverlay->GetWidgetFromName(TEXT("AmmoTypeIcon")));
		UImage* DefaultWeaponWidget = Cast<UImage>(BlasterHUD->CharacterOverlay->GetWidgetFromName(TEXT("WeaponTypeIcon")));

		if (AmmoIcon && WeaponIcon && BlasterHUD)
		{
			AmmoImageWidget->SetBrushFromTexture(AmmoIcon);
			WeaponImageWidget->SetBrushFromTexture(WeaponIcon);
		}

		FString EquippedWeaponType;
		switch (WeaponType)
		{
		case EWeaponType::EWT_AssaultRifle:
			EquippedWeaponType = FString::Printf(TEXT("Assault Rifle"));
			break;
		case EWeaponType::EWT_RocketLauncher:
			EquippedWeaponType = FString::Printf(TEXT("Rocket Launcher"));
			break;
		case EWeaponType::EWT_Pistol:
			EquippedWeaponType = FString::Printf(TEXT("Pistol"));
			break;
		case EWeaponType::EWT_SMG:
			EquippedWeaponType = FString::Printf(TEXT("SMG"));
			break;
		case EWeaponType::EWT_M4AZ:
			EquippedWeaponType = FString::Printf(TEXT("M4AZapper"));
			break;
		case EWeaponType::EWT_Shotgun:
			EquippedWeaponType = FString::Printf(TEXT("Shotgun"));
			break;
		case EWeaponType::EWT_SniperRifle:
			EquippedWeaponType = FString::Printf(TEXT("Sniper Rifle"));
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			EquippedWeaponType = FString::Printf(TEXT("Grenade Launcher"));
			break;
		case EWeaponType::EWT_MAX:
			break;
		default:
			break;
		}
		BlasterHUD->CharacterOverlay->WeaponType->SetText(FText::FromString(EquippedWeaponType));
	}
	else
	{
		bInitializeWeaponType = true;
		HUDWeaponType = WeaponType;
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt((CountdownTime - 5.f) / 60.f);
		int32 Seconds = FMath::Max(0, FMath::FloorToInt((CountdownTime - 5.f) - Minutes * 60.f));


		if (Minutes == 0 && Seconds <= 0)
		{
			FString CountdownText = FString::Printf(TEXT("00:00"));

			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
			if (BlasterCharacter) BlasterCharacter->bDisableGameplay = true;

		}
		else if (Minutes >= 0 && Seconds >= 0)
		{
			FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
		}
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->Announcement &&
		BlasterHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDGrenades(int32 Grenades)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->GrenadeText;

	if (bHUDValid)
	{
		FString GrenadeText = FString::Printf(TEXT("%d"), Grenades);
		BlasterHUD->CharacterOverlay->GrenadeText->SetText(FText::FromString(GrenadeText));
	}
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	ABlasterGameState* GameState = GetWorld()->GetGameState<ABlasterGameState>();
	if (!GameState) return;

	float TimeElapsedInGame = GameState->GetTimeElapsed();

	if (HasAuthority())
	{
		//UE_LOG(LogTemp, Error, TEXT("TimeElapsed for Server %f"), TimeElapsedInGame);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("TimeElapsed for CLIENT %f"), TimeElapsedInGame);
	}


	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		if (GameState->HasMatchEndedAbruptly())
		{
			TimeLeft = CooldownTime + WarmupTime + TimeElapsedInGame - GetServerTime() + LevelStartingTime;
		}
		else
		{
			TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
		}
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeWeaponType) SetHUDWeaponType(HUDWeaponType);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (bShowTeamScores) InitTeamScores();
				if (bShowFlagIcons) InitFlagIcons();
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter && BlasterCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Quit", IE_Pressed, this, &ABlasterPlayerController::ShowReturnToMainMenu);
	InputComponent->BindAction("Chat", IE_Pressed, this, &ABlasterPlayerController::ToggleInputChatBox);

	InputComponent->BindAction("Stats", IE_Pressed, this, &ABlasterPlayerController::ShowMatchStats);
	InputComponent->BindAction("Stats", IE_Released, this, &ABlasterPlayerController::HideMatchStats);
}


void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}


float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch, bool bCaptureTheFlagMatch)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch, bCaptureTheFlagMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::SetCharacterOverlayVisibility(bool BCharOverlayVisible)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay;

	if (bHUDValid)
	{
		if (BCharOverlayVisible == true)
		{
			CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleMatchHasStarted(bool bTeamsMatch, bool bCaptureTheFlagMatch)
{
	if (HasAuthority()) bShowTeamScores = bTeamsMatch;
	if (HasAuthority()) bShowFlagIcons = bCaptureTheFlagMatch;
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay == nullptr)
		{
			BlasterHUD->AddCharacterOverlay();
		}
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (BlasterHUD->PlayerStats == nullptr)
		{
			BlasterHUD->AddPlayerStats();
		}
		if (bShowFlagIcons)
		{
			InitTeamScores();
			InitFlagIcons();
		}
		else if (bShowTeamScores)
		{
			InitTeamScores();
			HideTeamFlagIcons();
		}
		else
		{
			HideTeamScores();
			HideTeamFlagIcons();
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->RemoveFromParent();
		}		
		bool bHUDValid = BlasterHUD->Announcement &&
			BlasterHUD->Announcement->AnnouncementText &&
			BlasterHUD->Announcement->InfoText;

		if (bHUDValid)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = Announcement::NewMatchStartsIn;
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();

		

			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString = bShowTeamScores ? GetTeamsInfoText(BlasterGameState) : GetInfoText(TopPlayers);

				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));

			}	
		}
	}

	ShowVotingSystem();

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

FString ABlasterPlayerController::GetInfoText(const TArray<class ABlasterPlayerState*>& Players)
{
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (BlasterPlayerState == nullptr) return FString();
	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (Players.Num() == 1 && Players[0] == BlasterPlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if (Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		InfoTextString = Announcement::PlayersTiedForTheWin;
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}
	}
	return InfoTextString;
}

FString ABlasterPlayerController::GetTeamsInfoText(ABlasterGameState* BlasterGameState)
{
	if (BlasterGameState == nullptr) return FString();
	FString InfoTextString;

	const int32 RedTeamScore = BlasterGameState->RedTeamScore;
	const int32 BlueTeamScore = BlasterGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin);
		InfoTextString.Append(Announcement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Announcement::BlueTeam);
		InfoTextString.Append(TEXT("\n"));
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Announcement::RedTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoTextString = Announcement::BlueTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
	}

	return InfoTextString;
}

void ABlasterPlayerController::ShowMatchStats()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->ScoresOverview &&
		BlasterHUD->ScoresOverview->PlayerStats;
	if (bHUDValid)
	{
		BlasterHUD->ScoresOverview->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->ScoresOverview->PlayerStats->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterPlayerController::HideMatchStats()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->ScoresOverview &&
		BlasterHUD->ScoresOverview->PlayerStats;


	if (bHUDValid)
	{
		BlasterHUD->ScoresOverview->SetVisibility(ESlateVisibility::Hidden);
		BlasterHUD->ScoresOverview->PlayerStats->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABlasterPlayerController::UpdateSpeedBuffIcon_Implementation(bool bIsBuffActive)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->SpeedBuffIcon;

	//UE_LOG(LogTemp, Warning, TEXT("UpdateSpeedBuffIcon: %s"), bIsBuffActive ? TEXT("true") : TEXT("false"));

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->SpeedBuffIcon)
	{
		BlasterHUD->CharacterOverlay->SpeedBuffIcon->SetBrushFromTexture(bIsBuffActive ? SpeedBuffIconOn : SpeedBuffIconOff);
	}
}

void ABlasterPlayerController::UpdateJumpBuffIcon_Implementation(bool bIsBuffActive)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->JumpBuffIcon;

	//UE_LOG(LogTemp, Warning, TEXT("UpdateJumpBuffIcon: %s"), bIsBuffActive ? TEXT("true") : TEXT("false"));

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->JumpBuffIcon)
	{
		BlasterHUD->CharacterOverlay->JumpBuffIcon->SetBrushFromTexture(bIsBuffActive ? JumpBuffIconOn : JumpBuffIconOff);
	}
}

void ABlasterPlayerController::UpdateBerserkBuffIcon_Implementation(bool bIsBuffActive)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BerserkBuffIcon;

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->BerserkBuffIcon)
	{
		BlasterHUD->CharacterOverlay->BerserkBuffIcon->SetBrushFromTexture(bIsBuffActive ? BerserkBuffIconOn : BerserkBuffIconOff);

		//UE_LOG(LogTemp, Warning, TEXT("Controller %s updated BerserkBuffIcon to %s"), *GetName(), bIsBuffActive ? TEXT("On") : TEXT("Off"));
	}

}


