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
    AddToViewport(5);
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        BlasterPlayerState = Cast<ABlasterPlayerState>(PlayerController->PlayerState);
        BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(World));

        if (PlayerController)
        {
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            BlasterPlayerController->SetInputMode(InputModeData);
            BlasterPlayerController->SetShowMouseCursor(true);
        }
        if (BlasterGameState)
        {
            BlasterGameState->OnFFAVoteCast.AddDynamic(this, &UVotingSyastem::UpdateFFAText);
            BlasterGameState->OnFFASMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateFFASMText);
            BlasterGameState->OnTDMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateTDMText);
            BlasterGameState->OnTDMSMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateTDMSMText);
            BlasterGameState->OnCTFVoteCast.AddDynamic(this, &UVotingSyastem::UpdateCTFText);
            BlasterGameState->OnCTFSMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateCTFSMText);
            BlasterGameState->OnInstaKillVoteCast.AddDynamic(this, &UVotingSyastem::UpdateInstaKillText);
            BlasterGameState->OnInstaKillSMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateInstaKillSMText);
        }
    }

    if (FFAButton)
    {
        FFAButton->OnClicked.AddDynamic(this, &UVotingSyastem::FFAVoteCast);
        FFASMButton->OnClicked.AddDynamic(this, &UVotingSyastem::FFASMVoteCast);
        TDMButton->OnClicked.AddDynamic(this, &UVotingSyastem::TDMVoteCast);
        TDMSMButton->OnClicked.AddDynamic(this, &UVotingSyastem::TDMSMVoteCast);
        CTFButton->OnClicked.AddDynamic(this, &UVotingSyastem::CTFVoteCast);
        CTFSMButton->OnClicked.AddDynamic(this, &UVotingSyastem::CTFSMVoteCast);
        InstaKillButton->OnClicked.AddDynamic(this, &UVotingSyastem::InstaKillVoteCast);
        InstaKillSMButton->OnClicked.AddDynamic(this, &UVotingSyastem::InstaKillSMVoteCast);
    }
}

void UVotingSyastem::MenuTeardown()
{
    if (this == nullptr) return;
    RemoveFromParent();

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }

    //GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void UVotingSyastem::FFAVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
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

void UVotingSyastem::FFASMVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_FFASMVoteCast();
            UE_LOG(LogTemp, Warning, TEXT("The FFA button was clicked!"));
        }
    }

    // disable the button
    if (FFASMButton)
    {
        FFASMButton->SetIsEnabled(false);
    }
}

void UVotingSyastem::TDMVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
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

void UVotingSyastem::TDMSMVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_TDMSMVoteCast();
        }
    }

    // disable the button
    if (TDMSMButton)
    {
        TDMSMButton->SetIsEnabled(false);
    }
}

void UVotingSyastem::CTFVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
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

void UVotingSyastem::CTFSMVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_CTFSMVoteCast();
        }
    }

    // disable the button
    if (CTFSMButton)
    {
        CTFSMButton->SetIsEnabled(false);
    }
}

void UVotingSyastem::InstaKillVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
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

void UVotingSyastem::InstaKillSMVoteCast()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_InstaKillSMVoteCast();
        }
    }

    // disable the button
    if (InstaKillSMButton)
    {
        InstaKillSMButton->SetIsEnabled(false);
    }
}


void UVotingSyastem::UpdateFFAText(int32 Vote)
{
    if (FFATotalVotesText)
    {
        FFATotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateFFASMText(int32 Vote)
{
    if (FFASMTotalVotesText)
    {
        FFASMTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateTDMText(int32 Vote)
{
    if (TDMTotalVotesText)
    {
        TDMTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateTDMSMText(int32 Vote)
{
    if (TDMSMTotalVotesText)
    {
        TDMSMTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateCTFText(int32 Vote)
{
    if (CTFTotalVotesText)
    {
        CTFTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateCTFSMText(int32 Vote)
{
    if (CTFSMTotalVotesText)
    {
        CTFSMTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateInstaKillText(int32 Vote)
{
    if (InstaKillTotalVotesText)
    {
        InstaKillTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}

void UVotingSyastem::UpdateInstaKillSMText(int32 Vote)
{
    if (InstaKillSMTotalVotesText)
    {
        InstaKillSMTotalVotesText->SetText(FText::FromString(FString::FromInt(Vote)));
    }
}
