// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/SpinBox.h"
#include "ServerListLine.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"


void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath, FString ServerNameChosen)
{

	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	ServerName = ServerNameChosen;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);

	}

	if (DMMatchTimeBox)
	{
		//DMMatchTimeBox->OnValueCommitted.AddDynamic(this, &ThisClass::OnDMMatchTimeValueChanged);
	}
	if (ServerIDInputBox)
	{
		ServerIDInputBox->OnTextChanged.AddDynamic(this, &ThisClass::OnServerIDConfirmed);
	}
}

void UMenu::OnServerIDConfirmed(const FText& ServerIDText)
{
	FString ServerIDString = ServerIDText.ToString();
	FOnlineSessionSearchResult* SessionToJoin = nullptr;

	// Find the correct session based on ServerID
	for (FOnlineSessionSearchResult& SessionSearchResult : *SessionSearchResults)
	{
		if (SessionSearchResult.GetSessionIdStr() == ServerIDString)
		{
			SessionToJoin = &SessionSearchResult;
			break;
		}
	}

	// If a matching session was found, attempt to join it
	if (SessionToJoin != nullptr)
	{
		MultiplayerSessionsSubsystem->JoinSession(*SessionToJoin);
	}
	else
	{
		// Handle case where no matching session was found
		return;
	}
}

void UMenu::RefreshServerList()
{
	if (ServerSelectionBox)
	{
		ServerSelectionBox->ClearChildren();
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->FindSessions(10000);
		}
		if (SessionSearchResults != nullptr && SessionSearchResults->Num() > 0)
		{
			for (FOnlineSessionSearchResult& SessionSearchResult : *SessionSearchResults)
			{
				ServerListLineWidget = CreateWidget<UServerListLine>(GetWorld(), ServerListLine);

				if (ServerListLineWidget == nullptr) continue;

				ServerID = SessionSearchResult.GetSessionIdStr();

				ServerListLineWidget->HostNameText->SetText(FText::FromString(SessionSearchResult.Session.OwningUserName));
				ServerListLineWidget->MaxPlayersText->SetText(FText::FromString(FString::FromInt(SessionSearchResult.Session.SessionSettings.NumPublicConnections)));
				ServerListLineWidget->ServerPingText->SetText(FText::FromString(FString::FromInt(SessionSearchResult.PingInMs)));
				ServerListLineWidget->ServerIDText->SetText(FText::FromString(SessionSearchResult.GetSessionIdStr()));
				ServerSelectionBox->AddChild(ServerListLineWidget);

			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString(TEXT("SessionResults was either nullptr or no sessions were found nigga")));
		}
	}
}

void UMenu::OnDMMatchTimeValueChanged(float InValue)
{

}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	if (TrainingButton)
	{
		TrainingButton->OnClicked.AddDynamic(this, &ThisClass::TrainingButtonClicked);
	}

	if (RefreshServers)
	{
		RefreshServers->OnClicked.AddDynamic(this, &ThisClass::RefreshServerList);
	}
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString(TEXT("Session Created Successfully")));
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
			}
			HostButton->SetIsEnabled(true);
		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	// Allocate memory for SessionSearchResults and assign the SessionResults.
	SessionSearchResults = new TArray<FOnlineSessionSearchResult>(SessionResults);

	for (const auto& Result : *SessionSearchResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			//MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}



void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		//FServerData ServerDataCreation;
		//ServerDataCreation.ConnNum = 5;
		//ServerDataCreation.ServerName = ServerNameTextBox->GetText().ToString();
		//ServerDataCreation.MatchType = ("FreeForAllSM");
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType, ServerName);
		UE_LOG(LogTemp, Error, TEXT("Match Type: %s"), *MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::TrainingButtonClicked()
{
	TrainingButton->SetIsEnabled(false);
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Maps/TrainingMap");
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}