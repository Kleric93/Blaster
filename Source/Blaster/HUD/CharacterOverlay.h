// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* YouDiedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClientJoinedText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerEliminatedText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamScore;

	UPROPERTY(meta = (BindWidget))
	class UImage* BlueFlagState;

	UPROPERTY(meta = (BindWidget))
	UImage* RedFlagState;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeamScoresText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Spacer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeText;

	UPROPERTY(meta = (BindWidget))
	class UImage* HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HighPingAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ClientJoinAnim;

	UPROPERTY(meta = (BindWidget))
		class UImage* EventBorderImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* EventBorderAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* DamageBorderAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* DeathBorderAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* PlayerEliminatedAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* DamageIndicatorAnim;

	UPROPERTY(meta = (BindWidget))
	UImage* SpeedBuffIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* JumpBuffIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* BerserkBuffIcon;

	UPROPERTY(meta = (BindWidget))
		UImage* DamageIndicator;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreToWin;
};
