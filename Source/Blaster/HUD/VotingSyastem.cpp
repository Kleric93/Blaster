// Fill out your copyright notice in the Description page of Project Settings.


#include "VotingSyastem.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"


void UVotingSyastem::NativeConstruct()
{
    Super::NativeConstruct();
}

void UVotingSyastem::MenuSetup()
{
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        BlasterPlayerState = Cast<ABlasterPlayerState>(PlayerController->PlayerState);
        BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(World));

        if (PlayerController)
        {
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
        if (BlasterGameState)
        {
            BlasterGameState->OnFFAVoteCast.AddDynamic(this, &UVotingSyastem::UpdateFFAText);
            BlasterGameState->OnTDMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateTDMText);
            BlasterGameState->OnCTFVoteCast.AddDynamic(this, &UVotingSyastem::UpdateCTFText);
            BlasterGameState->OnInstaKillVoteCast.AddDynamic(this, &UVotingSyastem::UpdateInstaKillText);
        }
    }

    if (FFAButton)
    {
        FFAButton->OnClicked.AddDynamic(this, &UVotingSyastem::FFAVoteCast);
        TDMButton->OnClicked.AddDynamic(this, &UVotingSyastem::TDMVoteCast);
        CTFButton->OnClicked.AddDynamic(this, &UVotingSyastem::CTFVoteCast);
        InstaKillButton->OnClicked.AddDynamic(this, &UVotingSyastem::InstaKillVoteCast);
    }
}

void UVotingSyastem::MenuTeardown()
{
    RemoveFromParent();
    bIsFocusable = false;

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        BlasterPlayerState = Cast<ABlasterPlayerState>(PlayerController->PlayerState);
        BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(World));

        if (PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }

    if (BlasterGameState)
    {
        BlasterGameState->OnFFAVoteCast.RemoveDynamic(this, &UVotingSyastem::UpdateFFAText);
        BlasterGameState->OnTDMVoteCast.RemoveDynamic(this, &UVotingSyastem::UpdateTDMText);
        BlasterGameState->OnCTFVoteCast.RemoveDynamic(this, &UVotingSyastem::UpdateCTFText);
        BlasterGameState->OnInstaKillVoteCast.RemoveDynamic(this, &UVotingSyastem::UpdateInstaKillText);
    }

    if (FFAButton)
    {
        FFAButton->OnClicked.RemoveDynamic(this, &UVotingSyastem::FFAVoteCast);
        TDMButton->OnClicked.RemoveDynamic(this, &UVotingSyastem::TDMVoteCast);
        CTFButton->OnClicked.RemoveDynamic(this, &UVotingSyastem::CTFVoteCast);
        InstaKillButton->OnClicked.RemoveDynamic(this, &UVotingSyastem::InstaKillVoteCast);
    }
}

void UVotingSyastem::FFAVoteCast()
{
    if (PlayerController)
    {
        ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_FFAVoteCast();
            UE_LOG(LogTemp, Warning, TEXT("The FFA button was clicked!"));
        }
    }

    // disable the button
    if (FFAButton)
    {
        FFAButton->SetIsEnabled(false);
    }
}

void UVotingSyastem::TDMVoteCast()
{
    if (PlayerController)
    {
        ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_TDMVoteCast();
        }
    }

    // disable the button
    if (TDMButton)
    {
        TDMButton->SetIsEnabled(false);
    }
}

void UVotingSyastem::CTFVoteCast()
{
    if (PlayerController)
    {
        ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_CTFVoteCast();
        }
    }

    // disable the button
    if (CTFButton)
    {
        CTFButton->SetIsEnabled(false);
    }
}

void UVotingSyastem::InstaKillVoteCast()
{
    if (PlayerController)
    {
        ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_InstaKillVoteCast();
        }
    }

    // disable the button
    if (InstaKillButton)
    {
        InstaKillButton->SetIsEnabled(false);
    }
}


void UVotingSyastem::UpdateFFAText(int32 Vote)
{
    if (FFATotalVotesText)
    {
        FFATotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateTDMText(int32 Vote)
{
    if (TDMTotalVotesText)
    {
        TDMTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateCTFText(int32 Vote)
{
    if (CTFTotalVotesText)
    {
        CTFTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateInstaKillText(int32 Vote)
{
    if (InstaKillTotalVotesText)
    {
        InstaKillTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}