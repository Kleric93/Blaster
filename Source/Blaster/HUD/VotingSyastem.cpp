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
        if (BlasterPlayerState)
        {
            BlasterGameState->OnFFAVoteCast.AddDynamic(this, &UVotingSyastem::UpdateFFAText);
            BlasterGameState->OnTDMVoteCast.AddDynamic(this, &UVotingSyastem::UpdateTDMText);

        } 
    }

    if (FFAButton)
    {
        FFAButton->OnClicked.AddDynamic(this, &UVotingSyastem::FFAVoteCast);
        TDMButton->OnClicked.AddDynamic(this, &UVotingSyastem::TDMVoteCast);
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
