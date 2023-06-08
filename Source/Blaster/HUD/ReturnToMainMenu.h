// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

class UMultiplayerSessionsSubsystem;
class UKeymapLine;
class UButton;
class UScrollBox;
class APlayerController;
class ABlasterCharacter;
class UInputConfig;
class UPlayerMappableInputConfig;
class UBlasterUserSettings;

/**
 * 
 */
UCLASS()
class BLASTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void MenuSetup();

	UFUNCTION(BlueprintCallable)
	void MenuTearDown();

protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();

	void Rebinding();

	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);


private:

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ReturnButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* KeybindingsScrollBox;


	UFUNCTION()
	void ReturnButtonClicked();

	UFUNCTION()
	void QuitButtonClicked();

	UFUNCTION()
	void OptionsButtonClicked();

	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UKeymapLine> KeymapLine;

	UPROPERTY()
	UKeymapLine* KeymapLineWidget;

	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	UInputConfig* InputConfig;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPlayerMappableInputConfig> PMIConfigKeyboard;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UPlayerMappableInputConfig> PMIConfigController;

	TMap<UKeymapLine*, FName> WidgetToActionMap;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class USettingsMenu> SettingsMenu;

	UPROPERTY()
		USettingsMenu* SettingsMenuWidget;

	UPROPERTY()
		UBlasterUserSettings* Settings;
};
