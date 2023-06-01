// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterUserSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "InputCoreTypes.h"


UBlasterUserSettings::UBlasterUserSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseTurnRate = 1.f;
	AimingTurnRateForShortZoom = 1.4f;
	AimingTurnRateForLongZoom = 3.f;
	bLocalPlayerOverheadWidgetVisibility = false;
	MasterSoundVolume = 0.5f;
	MusicSoundVolume = 0.5f;
	GameTime = 10.f;
	MaxScore = 10.f;
	ScoreToWin = 30;
	TeamChosen = ETeam::ET_NoTeam;

}


void UBlasterUserSettings::SetBaseTurnRate(float InBaseTurnRate)
{
	BaseTurnRate = InBaseTurnRate;
}

float UBlasterUserSettings::GetBaseTurnRate() const
{
	return BaseTurnRate;
}

void UBlasterUserSettings::SetAimingTurnRateForShortZoom(float InAimingTurnRateForShortZoom)
{
	AimingTurnRateForShortZoom = InAimingTurnRateForShortZoom;
}

float UBlasterUserSettings::GetAimingTurnRateForShortZoom() const
{
	return AimingTurnRateForShortZoom;
}

void UBlasterUserSettings::SetAimingTurnRateForLongZoom(float InAimingTurnRateForLongZoom)
{
	AimingTurnRateForLongZoom = InAimingTurnRateForLongZoom;
}

void UBlasterUserSettings::SetbLocalPlayerOverheadWidgetVisibility(bool bLocalOverheadWidgetVisible)
{
	bLocalPlayerOverheadWidgetVisibility = bLocalOverheadWidgetVisible;
}

void UBlasterUserSettings::SetTeamToChoose(ETeam TeamOfChoice)
{
	TeamChosen = TeamOfChoice;
}

ETeam UBlasterUserSettings::GetTeamToChoose() const
{
	return TeamChosen;
}

void UBlasterUserSettings::SetCustomKeyboardConfig(TMap<FName, FKey> CustomMKConfig)
{
	CustomKeyboardConfig = CustomMKConfig;
}

TMap<FName, FKey> UBlasterUserSettings::GetCustomKeyboardConfig() const
{
	return CustomKeyboardConfig;
}

void UBlasterUserSettings::SetMaxScore(float InMaxScore)
{
	MaxScore = InMaxScore;
}

bool UBlasterUserSettings::GetbLocalPlayerOverheadWidgetVisibility() const
{
	return bLocalPlayerOverheadWidgetVisibility;
}

float UBlasterUserSettings::GetAimingTurnRateForLongZoom() const
{
	return AimingTurnRateForLongZoom;
}

float UBlasterUserSettings::GetGameTime() const
{
	return GameTime;
}

UBlasterUserSettings* UBlasterUserSettings::GetBlasterUserSettings()
{
	return Cast<UBlasterUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UBlasterUserSettings::SetMasterSoundVolume(float NewValue)
{
	MasterSoundVolume = NewValue;
}

float UBlasterUserSettings::GetMasterSoundVolume()
{
	return MasterSoundVolume;

	UE_LOG(LogTemp, Error, TEXT("MasterVolume: %f"), MasterSoundVolume);

}

void UBlasterUserSettings::SetMusicSoundVolume(float NewValue)
{
	MusicSoundVolume = NewValue;

}

float UBlasterUserSettings::GetMusicSoundVolume()
{
	return MusicSoundVolume;
}