// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnToMainMenu.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Blaster/InputConfig.h"
#include "KeymapLine.h"
#include "Components/TextBlock.h"
#include "InputAction.h"
#include "Components/InputKeySelector.h"
#include "Components/ScrollBox.h"
#include "PlayerMappableInputConfig.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/SettingsMenu.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/BlasterUserSettings.h"

void UReturnToMainMenu::MenuSetup()
{
	AddToViewport(3);
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	Rebinding();

	if (ResumeButton && !ResumeButton->OnClicked.IsBound())
	{
		ResumeButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::MenuTearDown);
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	if (QuitButton && !QuitButton->OnClicked.IsBound())
	{
		QuitButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::QuitButtonClicked);
	}

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::OptionsButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);
		}
	}
}

bool UReturnToMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
	}
	
	return true;
}



void UReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UReturnToMainMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		ABlasterPlayerController* PC = Cast<ABlasterPlayerController>(PlayerController);
			if (PlayerController)
			{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
			}
			if (PC)
			{
				PC->bReturnToMainMenuOpen = false;
			}
	}
	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}
	if (QuitButton && QuitButton->OnClicked.IsBound())
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::QuitButtonClicked);
	}
	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
	}
}

void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();
		if (FirstPlayerController)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if (BlasterCharacter)
			{
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
		//World->ServerTravel(FString("/Game/Maps/BaseMap"), true); // causes crashes.
	}
}

void UReturnToMainMenu::QuitButtonClicked()
{
	QuitButton->SetIsEnabled(false);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();
		if (FirstPlayerController)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if (BlasterCharacter)
			{
				BlasterCharacter->Elim(true);
				BlasterCharacter->MulticastElim(true);
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);

				// Add a delay of 2 seconds before quitting the game
				FTimerHandle TimerHandle;
				World->GetTimerManager().SetTimer(TimerHandle, [FirstPlayerController]() {
					FirstPlayerController->ConsoleCommand("quit");
					}, 5.0f, false);
			}
			else
			{
				QuitButton->SetIsEnabled(true);
			}
		}
	}
}

void UReturnToMainMenu::OptionsButtonClicked()
{
	if (SettingsMenuWidget == nullptr)
	{
		SettingsMenuWidget = CreateWidget<USettingsMenu>(GetWorld(), SettingsMenu);
	}
	MenuTearDown();
	SettingsMenuWidget->MenuSetup();
}


void UReturnToMainMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UReturnToMainMenu::Rebinding()
{
	if (KeybindingsScrollBox)
	{
		if (Settings == nullptr)
		{
			Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
		}
		KeybindingsScrollBox->ClearChildren();

		Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwningPlayerPawn()) : Character;

		if (Character == nullptr) return;

		//InputConfig = Cast<UInputConfig>(Character->GetInputConfig());
		//if (InputConfig == nullptr) return;
		FText ConfigName = Config->GetDisplayName();
		UE_LOG(LogTemp, Error, TEXT("CONFIG NAME: %s"), *ConfigName.ToString());


		// Proceed only if Character, InputConfig, and PlayermappableConfig are not nullptr
		if (Config)
		{
			// Populate the keybinding scrollbox
			for (const FEnhancedActionKeyMapping& Mapping : Config->GetPlayerMappableKeys())
			{
				KeymapLineWidget = CreateWidget<UKeymapLine>(GetOwningPlayer(), KeymapLine);
				if (KeymapLineWidget == nullptr) continue;

				// Set the name of the action
				// Set the name of the action
				if (Mapping.PlayerMappableOptions.Name != NAME_None)
				{
					Settings->GetCustomKeyboardConfig().Add(Mapping.PlayerMappableOptions.Name, Mapping.Key);
					KeymapLineWidget->KeyName->SetText(Mapping.PlayerMappableOptions.DisplayName);

					KeymapLineWidget->KeyboardKeySelector->OnKeySelected.AddDynamic(this, &UReturnToMainMenu::OnKeySelected);
					WidgetToActionMap.Add(KeymapLineWidget, Mapping.PlayerMappableOptions.Name);
				}


				KeymapLineWidget->KeyboardKeySelector->OnKeySelected.AddDynamic(this, &UReturnToMainMenu::OnKeySelected);


				// Initialize the key selectors
				// This is a placeholder, you would need to fetch the actual keys associated with the action
				FKey DefaultKey = Mapping.Key;
				FName DefaultMappingName = Mapping.Key.GetFName();

				// Bind OnKeySelectedDelegate to OnKeySelected function

				// Set the selected key
				KeymapLineWidget->KeyboardKeySelector->SetSelectedKey(DefaultKey);

				KeybindingsScrollBox->AddChild(KeymapLineWidget);

				// TODO: Bind the reset button to a function that resets the keybinds to the default ones

				/*
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetOwningLocalPlayer()))
				{
					Subsystem->AddPlayerMappedKey(DefaultMappingName, DefaultKey);
				}*/

				KeybindingsScrollBox->AddChild(KeymapLineWidget);
			}


		}
	}
}

void UReturnToMainMenu::OnKeySelected(FInputChord SelectedKey)
{
	UE_LOG(LogTemp, Error, TEXT("KeySelectedDelegate"));
	if (Settings == nullptr)
	{
		Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
	}

	FKey NewKey = SelectedKey.Key;
	FName ActionName = WidgetToActionMap[KeymapLineWidget];
	if (Settings->GetCustomKeyboardConfig().Contains(ActionName))
	{
		Settings->GetCustomKeyboardConfig()[ActionName] = NewKey;
	}
	else
	{
		Settings->GetCustomKeyboardConfig().Add(ActionName, NewKey);
	}
	// And also update the mapped key in the subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetOwningLocalPlayer()))
	{
		Subsystem->AddPlayerMappedKey(ActionName, NewKey);
	}
}

// SAFEKEEPING
/*
* 
* 
void UReturnToMainMenu::Rebinding()
{
	if (KeybindingsScrollBox)
	{
		if (Settings == nullptr)
		{
			Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
		}
		KeybindingsScrollBox->ClearChildren();

		Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwningPlayerPawn()) : Character;

		if (Character == nullptr) return;

		//InputConfig = Cast<UInputConfig>(Character->GetInputConfig());
		//if (InputConfig == nullptr) return;
		FText ConfigName = Config->GetDisplayName();
		UE_LOG(LogTemp, Error, TEXT("CONFIG NAME: %s"), *ConfigName.ToString());


		// Proceed only if Character, InputConfig, and PlayermappableConfig are not nullptr
		if (Config)
		{
			// Populate the keybinding scrollbox
			for (const FEnhancedActionKeyMapping& Mapping : Config->GetPlayerMappableKeys())
			{
				KeymapLineWidget = CreateWidget<UKeymapLine>(GetOwningPlayer(), KeymapLine);
				if (KeymapLineWidget == nullptr) continue;

				// Set the name of the action
				if (Mapping.PlayerMappableOptions.Name != NAME_None)
				{

					Settings->GetCustomKeyboardConfig().Add(Mapping.PlayerMappableOptions.Name, Mapping.Key);
					KeymapLineWidget->KeyName->SetText(Mapping.PlayerMappableOptions.DisplayName);
				}

				KeymapLineWidget->KeyboardKeySelector->OnKeySelected.AddDynamic(this, &UReturnToMainMenu::OnKeySelected);


				// Initialize the key selectors
				// This is a placeholder, you would need to fetch the actual keys associated with the action
				FKey DefaultKey = Mapping.Key;
				FName DefaultMappingName = Mapping.Key.GetFName();

				// Bind OnKeySelectedDelegate to OnKeySelected function

				// Set the selected key
				KeymapLineWidget->KeyboardKeySelector->SetSelectedKey(DefaultKey);

				KeybindingsScrollBox->AddChild(KeymapLineWidget);

				// TODO: Bind the reset button to a function that resets the keybinds to the default ones

				/*
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetOwningLocalPlayer()))
				{
					Subsystem->AddPlayerMappedKey(DefaultMappingName, DefaultKey);
				}

KeybindingsScrollBox->AddChild(KeymapLineWidget);
			}


		}
	}
}


void UReturnToMainMenu::Rebinding()
{

	if (KeybindingsScrollBox)
	{
		KeybindingsScrollBox->ClearChildren();

		Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwningPlayerPawn()) : Character;

		if (Character == nullptr) return;

		InputConfig = Cast<UInputConfig>(Character->GetInputConfig());
		PlayermappableConfig = Cast<UPlayerMappableInputConfig>(InputConfig);

		if (InputConfig == nullptr) return;

		if (Character && InputConfig)
		{
			// Populate the keybinding scrollbox
			for (const FTaggedInputAction& TaggedAction : InputConfig->TaggedInputActions)
			{
				KeymapLineWidget = CreateWidget<UKeymapLine>(GetOwningPlayer(), KeymapLine);
				if (KeymapLineWidget == nullptr) continue;

				// Set the name of the action
				KeymapLineWidget->KeyName->SetText(FText::FromName(TaggedAction.InputAction->GetFName()));

				// Initialize the key selectors
				// This is a placeholder, you would need to fetch the actual keys associated with the action
				FInputChord InitialKey;
				KeymapLineWidget->KeyboardKeySelector->SetSelectedKey(InitialKey);
				KeymapLineWidget->GamePadKeySelector->SetSelectedKey(InitialKey);

				// TODO: Bind the reset button to a function that resets the keybinds to the default ones

				KeybindingsScrollBox->AddChild(KeymapLineWidget);
			}
		}
	}
}


	if (InputConfigName != TEXT("Custom"))
	{
		// Copy Presets.
		if (const UPlayerMappableInputConfig* DefaultConfig = GetInputConfigByName(TEXT("Default")))
		{
			for (const FEnhancedActionKeyMapping& Mapping : DefaultConfig->GetPlayerMappableKeys())
			{
				// Make sure that the mapping has a valid name, its possible to have an empty name
				// if someone has marked a mapping as "Player Mappable" but deleted the default field value
				if (Mapping.PlayerMappableOptions.Name != NAME_None)
				{
					CustomKeyboardConfig.Add(Mapping.PlayerMappableOptions.Name, Mapping.Key);
				}
			}
		}

		InputConfigName = TEXT("Custom");
	}
*/