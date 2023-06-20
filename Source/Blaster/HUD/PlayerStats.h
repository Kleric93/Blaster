// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/Team.h"
#include "PlayerStats.generated.h"

/**
 *
 */
UCLASS()
class BLASTER_API UPlayerStats : public UUserWidget
{
	GENERATED_BODY()


public:


	UPROPERTY(meta = (BindWidget))
		class UScrollBox* PlayerStatsScrollBox;

	UPROPERTY(meta = (BindWidget))
		UScrollBox* PlayerStatsScrollBox_RedTeam;

	UPROPERTY(meta = (BindWidget))
		UScrollBox* PlayerStatsScrollBox_BlueTeam;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlayerStatsLine> PlayerStatLine;

	UPROPERTY()
		UPlayerStatsLine* PlayerStatsLineWidget;

	UPROPERTY(meta = (BindWidget))
		class UOverlay* BlueTeamOverlay;

	UPROPERTY(meta = (BindWidget))
		class UOverlay* RedTeamOverlay;

	UPROPERTY(meta = (BindWidget))
		class UOverlay* NoTeamOverlay;

	void WidgetSetup(TArray<class ABlasterPlayerState*> BlasterPlayerStateArray);

	void WidgetSetupTeams(TArray<class ABlasterPlayerState*> BlasterPlayerStateArray);

	void SetupPlayerStatsLineWidget(ABlasterPlayerState* BlasterPlayerState, const FString& PlayerName);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateKills(const FString& PlayerName, int32 NewKills);

	UPlayerStatsLine* FindPlayerStatsLine(const FString& PlayerName);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateDeaths(const FString& PlayerName, int32 NewDeaths);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateKD(const FString& PlayerName, int32 Kills, int32 Deaths);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateTeam(const FString& PlayerName, ETeam TeamToAssign);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateTeamScorePoints(const FString& PlayerName, ETeam TeamthatScored, int32 PlayerScore);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void UpdateOverallScores(ETeam TeamThatScored, int32 PointScored);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void RemovePlayerFromScoreboard(ABlasterPlayerState* PlayerLeaving);

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void AddPlayerToScoreboard(ABlasterPlayerState* PlayerJoining, const FString& PlayerName);


	bool IsLocalPlayer(const FString& PlayerName);

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RedTeamScore;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BlueTeamScore;

	UPROPERTY(EditAnywhere)
		class UTexture2D* NoTeamIcon;

	UPROPERTY(EditAnywhere)
		class UTexture2D* RedTeamIcon;

	UPROPERTY(EditAnywhere)
		class UTexture2D* BlueTeamIcon;

	UPROPERTY()
		TMap<FString, int32> PlayerScores;

	UPROPERTY()
		TMap<FString, UPlayerStatsLine*> PlayerStatsWidgets;
};