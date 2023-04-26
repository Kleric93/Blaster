// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Pickups/FlagStates.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:


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

	void AddPlayerStats();

	void HandleCooldown();

	float SingleTripTime = 0;

	FHighPingDelegate HighPingDelegate;

	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	void HideTeamScores();
	void HideTeamFlagIcons();
	void InitTeamScores();
	void InitFlagIcons();
	void SetHUDRedTeamScore(int32 RedScore);
	void SetHUDBlueTeamScore(int32 BlueScore);

	void SetHUDBlueFlagState(AActor* Flag, EFlagState NewFlagState);
	void SetHUDRedFlagState(AActor* Flag, EFlagState NewFlagState);

	void UpdateBlueFlagStateInHUD(EFlagState NewFlagState);
	void UpdateRedFlagStateInHUD(EFlagState NewFlagState);

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
	
private:
	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UChatSystemOverlay> ChatSystemOverlayClass;
	UPROPERTY()
	UChatSystemOverlay* ChatSystemWidget;


protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void SetHUDTime();
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
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);


	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

	void ShowReturnToMainMenu();

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

	bool bReturnToMainMenuOpen = false;

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;


	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
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

};
