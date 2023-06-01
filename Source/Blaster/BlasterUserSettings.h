// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "BlasterTypes/Team.h"
#include "BlasterUserSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class BLASTER_API UBlasterUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

public:

	//			//
	/// Aiming /// 
	//			//

	// GETTERS

	UFUNCTION(BlueprintPure)
		float GetBaseTurnRate() const;

	UFUNCTION(BlueprintPure)
		float GetAimingTurnRateForShortZoom() const;

	UFUNCTION(BlueprintPure)
		float GetAimingTurnRateForLongZoom() const;

	UFUNCTION(BlueprintPure)
		float GetGameTime() const;




	// SETTERS

	UFUNCTION(BlueprintCallable)
		void SetBaseTurnRate(float InBaseTurnRate);

	UFUNCTION(BlueprintCallable)
		void SetAimingTurnRateForShortZoom(float InAimingTurnRateForShortZoom);

	UFUNCTION(BlueprintCallable)
		void SetAimingTurnRateForLongZoom(float InAimingTurnRateForLongZoom);



	//				//
	///     Music	/// 
	//				//

	UFUNCTION(BlueprintCallable)
		void SetMasterSoundVolume(float NewValue);

	UFUNCTION(BlueprintPure)
		float GetMasterSoundVolume();

	UFUNCTION(BlueprintCallable)
		void SetMusicSoundVolume(float NewValue);

	UFUNCTION(BlueprintPure)
		float GetMusicSoundVolume();

	///
	//
	///

	UFUNCTION(BlueprintPure)
		float GetMaxScore() const { return MaxScore; }

	UFUNCTION(BlueprintCallable)
		void SetMaxScore(float InMaxScore);

	UFUNCTION(BlueprintPure)
		bool GetbLocalPlayerOverheadWidgetVisibility() const;

	UFUNCTION(BlueprintCallable)
		void SetbLocalPlayerOverheadWidgetVisibility(bool bLocalOverheadWidgetVisible);

	UFUNCTION(BlueprintCallable)
		void SetTeamToChoose(ETeam TeamOfChoice);

	UFUNCTION(BlueprintPure)
		ETeam GetTeamToChoose() const;

	void SetCustomKeyboardConfig(TMap<FName, FKey> CustomMKConfig);

	UFUNCTION(BlueprintPure)
	TMap<FName, FKey> GetCustomKeyboardConfig() const;

	UFUNCTION(BlueprintCallable)
		static UBlasterUserSettings* GetBlasterUserSettings();

protected:

	UPROPERTY(Config)
	float MasterSoundVolume;

	UPROPERTY(Config)
	float MusicSoundVolume;
	
	UPROPERTY(Config)
	float BaseTurnRate;

	UPROPERTY(Config)
	float AimingTurnRateForShortZoom;

	UPROPERTY(Config)
	float AimingTurnRateForLongZoom;

	UPROPERTY(Config, BlueprintReadWrite)
	float GameTime;

	UPROPERTY(Config, BlueprintReadWrite)
	float MaxScore;

	UPROPERTY(Config, BlueprintReadWrite)
	int32 ScoreToWin;

	UPROPERTY(Config)
		bool bLocalPlayerOverheadWidgetVisibility;

	UPROPERTY(Config)
		ETeam TeamChosen;

	UPROPERTY(Config)
		TMap<FName, FKey> CustomKeyboardConfig;
};
