// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Pickups/FlagStates.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterPlayerController.generated.h"

class UInputMappingContext;
class ABlasterPlayerState;
class USettingsMenu;
class UBlasterUserSettings;
class UTeamChoice;
class UInputAction;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamChosen, ABlasterPlayerController*, BPController, ETeam, ChosenTeam);



/**
 *
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABlasterPlayerController();

	UPROPERTY()
		UBlasterUserSettings* Settings;

	void IMCSelector(UInputMappingContext* MappingContexttoAdd, UInputMappingContext* MappingContexttoRemove);
	UInputMappingContext* GetKBMMappingContext() const { return BlasterMappingContextKBM; }
	UInputMappingContext* GetControllerMappingContext() const { return BlasterMappingContextController; }
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponType(EWeaponType WeaponType);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual float GetServerTime(); // synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible.
	void OnMatchStateSet(FName State, bool bTeamsMatch = false, bool bCaptureTheFlagMatch = false);
	void HandleMatchHasStarted(bool bTeamsMatch = false, bool bCaptureTheFlagMatch = false);
	void HandleCooldown();

	float SingleTripTime = 0;

	UPROPERTY(BlueprintAssignable, Category = "Score")
		FHighPingDelegate HighPingDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Teams")
		FOnTeamChosen OnTeamChosen;


	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	void SetMaxScore(float ScoreMax);

	void HideTeamScores();
	void HideTeamFlagIcons();
	void InitTeamScores();
	void InitFlagIcons();
	void SetHUDRedTeamScore(int32 RedScore);
	void SetHUDBlueTeamScore(int32 BlueScore);

	void SetHUDBlueFlagState(AActor* Flag, EFlagState NewFlagState);
	void SetHUDRedFlagState(AActor* Flag, EFlagState NewFlagState);

	UFUNCTION()
	void UpdateBlueFlagStateInHUD(EFlagState NewFlagState);
	UFUNCTION()
	void UpdateRedFlagStateInHUD(EFlagState NewFlagState);

	UFUNCTION(Client, Reliable)
	void UpdateJumpBuffIcon(bool bIsBuffActive);

	UFUNCTION(Client, Reliable)
	void UpdateSpeedBuffIcon(bool bIsBuffActive);

	UFUNCTION(Client, Reliable)
	void UpdateBerserkBuffIcon(bool bIsBuffActive);

	UFUNCTION(Client, Reliable)
		void EventBorderDamage();

	UFUNCTION(Client, Reliable)
		void EventBorderHeal();

	UFUNCTION(Client, Reliable)
		void EventBorderShield();

	UFUNCTION(Client, Reliable)
		void EventBorderPowerUp();

	UFUNCTION(Client, Reliable)
		void EventBorderDeath();

	UFUNCTION(Client, Reliable)
		void EventPlayerEliminated();
	//
	/// chat box
	//

	void AddChatBox();

	UFUNCTION()
		void ToggleInputChatBox();

	UFUNCTION()
		void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(Server, Reliable)
		void ServerSetText(const FString& Text, const FString& PlayerName);

	UFUNCTION(Client, Reliable)
		void ClientSetText(const FString& Text, const FString& PlayerName);

	FORCEINLINE float GetHUDScore() const { return HUDScore; }

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UVotingSyastem> VotingSystemWidget;

	UPROPERTY()
		class UVotingSyastem* VotingSystem;

	UFUNCTION(Server, Reliable)
		void Server_FFAVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_FFASMVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_TDMVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_TDMSMVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_CTFVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_CTFSMVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_InstaKillVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_InstaKillSMVoteCast();

	UFUNCTION(Server, Reliable)
		void Server_RedTeamChosen();

	UFUNCTION(Server, Reliable)
		void Server_BlueTeamChosen();

	UFUNCTION(Server, Reliable)
		void Server_UpdateScrollBoxes();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateScrollBoxes();

	void SetCharacterOverlayVisibility(bool BCharOverlayVisible);

	void SetHUDTime();

	void SetHUDDamageIndicator(float Angle);

	void SavePlayerInputMapping(FName MappingName, FKey Key);

	void UpdateInputMapping(FName MappingName, FKey Key);

	void LoadPlayerOverriddenInputMappings();

	bool bReturnToMainMenuOpen = false;
	bool bSettingsMenuOpen = false;

	FORCEINLINE float GetWarmupTime() const { return WarmupTime; };

	UPROPERTY(EditAnywhere)
		UInputAction* Jump;
private:

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UChatSystemOverlay> ChatSystemOverlayClass;
	UPROPERTY()
		UChatSystemOverlay* ChatSystemWidget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UTeamChoice> TeamChoiceClass;

	UPROPERTY()
		UTeamChoice* TeamChoiceWidget;


	UPROPERTY()
		FTimerHandle TimerHandle_CompareVotesAndLog;



protected:

	//
	/// Enhanced Input
	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BlasterMappingContextKBM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BlasterMappingContextController;

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void PollInit();
	virtual void SetupInputComponent() override;


	//
	/// SyncTime Between CLient and server
	//

	// requests the current server time, passing in the client's time when the request was sent.
	UFUNCTION(Server, Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);

	// reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	// Difference between client and server times
	float ClientServerDelta = 0;

	UPROPERTY(EditAnywhere, Category = Time)
		float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
		void ServerCheckmatchState();

	UFUNCTION(Client, Reliable)
		void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bIsTeamMatch, bool bIsCaptureTheFlagMatch);


	void HighPingWarning();
	void StopHighPingWarning();

	void CheckPing(float DeltaTime);

	UFUNCTION()
	void ShowReturnToMainMenu();

	UFUNCTION()
	void ShowVotingSystem();

	UFUNCTION(Client, Reliable)
		void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
		bool bShowTeamScores = false;

	UPROPERTY(ReplicatedUsing = OnRep_ShowFlagIcons)
		bool bShowFlagIcons = false;

	UFUNCTION()
		void OnRep_ShowTeamScores();

	UFUNCTION()
		void OnRep_ShowFlagIcons();

	FString GetInfoText(const TArray<class ABlasterPlayerState*>& Players);
	FString GetTeamsInfoText(class ABlasterGameState* BlasterGameState);

	void ShowMatchStats();

	void HideMatchStats();

private:

	//
	/// return to main menu
	//
	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
		class UReturnToMainMenu* ReturnToMainMenu;

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> SettingsMenuWidget;

	UPROPERTY()
		class USettingsMenu* SettingsMenu;


	UPROPERTY()
		class ABlasterHUD* BlasterHUD;

	UPROPERTY()
		class UPlayerStats* PlayerStats;

	UPROPERTY()
		class ABlasterGameMode* BlasterGameMode;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	bool bTMatch;
	bool bCTFMatch;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
		FName MatchState;

	UFUNCTION()
		void OnRep_MatchState();

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "True"))
		class UCharacterOverlay* CharacterOverlay;

	float HUDHealth;
	bool bInitializeHealth = false;
	float HUDMaxHealth;
	float HUDShield;
	bool bInitializeShield = false;
	float HUDMaxShield;
	float HUDScore;
	bool bInitializeScore = false;
	int32 HUDDefeats;
	bool bInitializeDefeats = false;
	EWeaponType HUDWeaponType;

	EFlagState FlagState;

	bool bInitializeWeaponType = false;
	int32 HUDGrenades;
	bool bInitializeGrenades = false;
	int32 HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;
	int32 HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float HUDScoreToWin;
	bool bInitializeScoreToWin = false;

	float highPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
		float HighPingDuration = 10.f;

	UPROPERTY(EditAnywhere)
		float CheckPingFrequency = 20.f;

	UFUNCTION(Server, Reliable)
		void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
		float HighPingThreshold = 50.f;

	float PingAnimationRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float VotingEndedTimer = 15.f;

	UPROPERTY(EditAnywhere)
		class UImage* DefaultWeaponIcon;
	UImage* DefaultAmmoIcon;

	UPROPERTY(EditAnywhere)
		class UTexture2D* BlueFlagInitial;

	UPROPERTY(EditAnywhere)
		UTexture2D* BlueFlagStolen;

	UPROPERTY(EditAnywhere)
		UTexture2D* BlueFlagDropped;

	UPROPERTY(EditAnywhere)
		UTexture2D* RedFlagInitial;

	UPROPERTY(EditAnywhere)
		UTexture2D* RedFlagStolen;

	UPROPERTY(EditAnywhere)
		UTexture2D* RedFlagDropped;

	UPROPERTY(EditAnywhere)
		UTexture2D* SpeedBuffIconOn;

	UPROPERTY(EditAnywhere)
		UTexture2D* SpeedBuffIconOff;

	UPROPERTY(EditAnywhere)
		UTexture2D* JumpBuffIconOn;

	UPROPERTY(EditAnywhere)
		UTexture2D* JumpBuffIconOff;

	UPROPERTY(EditAnywhere)
		UTexture2D* BerserkBuffIconOn;

	UPROPERTY(EditAnywhere)
		UTexture2D* BerserkBuffIconOff;

	FString InputMappingSlot = "InputMappingSlot";
};