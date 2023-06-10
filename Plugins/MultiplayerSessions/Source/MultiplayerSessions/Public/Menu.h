// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UButton;
class USpinBox;
class UEditableTextBox;
class UScrollBox;
class UServerListLine;

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")), FString ServerNameChosen = FString(TEXT("Server")));

	UFUNCTION()
		void OnDMMatchTimeValueChanged(float InValue);

protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom deleates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
		void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
		void OnStartSession(bool bWasSuccessful);

private:

	UPROPERTY(meta = (BindWidget))
		UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
		UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
		UButton* TrainingButton;

	UPROPERTY(meta = (BindWidget))
		UButton* RefreshServers;

	UPROPERTY(meta = (BindWidget))
		USpinBox* DMMatchTimeBox;

	UPROPERTY(meta = (BindWidget))
		USpinBox* DMScoreToWinBox;

	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* ServerNameTextBox;

	UPROPERTY(meta = (BindWidget))
		UScrollBox* ServerSelectionBox;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UServerListLine> ServerListLine;

	TArray<FOnlineSessionSearchResult>* SessionSearchResults;

	UPROPERTY()
		UServerListLine* ServerListLineWidget;

	UFUNCTION()
		void HostButtonClicked();

	UFUNCTION()
		void JoinButtonClicked();

	UFUNCTION()
		void TrainingButtonClicked();

	UFUNCTION()
	void RefreshServerList();


	void MenuTearDown();

	// the subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int32 NumPublicConnections {
		4
	};

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString MatchType { 
		TEXT("FreeForALl") 
	};

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FString ServerName {
		TEXT("ServerName")
	};

	FString PathToLobby{ TEXT("") };
};