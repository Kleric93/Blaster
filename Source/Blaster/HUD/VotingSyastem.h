// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VotingSyastem.generated.h"


class UCheckBox;
class UButton;
class UTextBlock;
class APlayerController;
class ABlasterGameState;
class ABlasterPlayerState;

/**
 *
 */
UCLASS()
class BLASTER_API UVotingSyastem : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		UButton* FFAButton;

	UPROPERTY(meta = (BindWidget))
		UButton* TDMButton;

	UPROPERTY(meta = (BindWidget))
		UButton* CTFButton;

	UPROPERTY(meta = (BindWidget))
		UButton* InstaKillButton;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* FFATotalVotesText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* TDMTotalVotesText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* CTFTotalVotesText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* InstaKillTotalVotesText;

	void MenuSetup();

	void MenuTeardown();

	UFUNCTION()
		void FFAVoteCast();

	UFUNCTION()
		void TDMVoteCast();

	UFUNCTION()
		void CTFVoteCast();

	UFUNCTION()
		void InstaKillVoteCast();

	UFUNCTION(BlueprintCallable)
		void UpdateFFAText(int32 Vote);

	UFUNCTION(BlueprintCallable)
		void UpdateTDMText(int32 Vote);

	UFUNCTION(BlueprintCallable)
		void UpdateCTFText(int32 Vote);

	UFUNCTION(BlueprintCallable)
		void UpdateInstaKillText(int32 Vote);


protected:

	virtual void NativeConstruct() override;



private:

	UPROPERTY()
		APlayerController* PlayerController;

	UPROPERTY()
		ABlasterGameState* BlasterGameState;

	UPROPERTY()
		ABlasterPlayerState* BlasterPlayerState;
};