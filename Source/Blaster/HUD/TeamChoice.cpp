// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamChoice.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UTeamChoice::WidgetSetup()
{
    AddToViewport(5);
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);

        if (PlayerController)
        {
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            BlasterPlayerController->SetInputMode(InputModeData);
            BlasterPlayerController->SetShowMouseCursor(true);
        }

        if (RedTeamButton)
        {
            RedTeamButton->OnClicked.AddDynamic(this, &UTeamChoice::OnRedButtonClicked);
            BlueTeamButton->OnClicked.AddDynamic(this, &UTeamChoice::OnBlueButtonClicked);
            CloseButton->OnClicked.AddDynamic(this, &UTeamChoice::WidgetTeardown);
        }

    }
}

void UTeamChoice::WidgetTeardown()
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
}

void UTeamChoice::OnRedButtonClicked()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_RedTeamChosen();
        }
    }

    // disable the button
    if (RedTeamButton)
    {
        RedTeamButton->SetIsEnabled(false);
        BlueTeamButton->SetIsEnabled(true);

    }
    GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}

void UTeamChoice::OnBlueButtonClicked()
{
    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_BlueTeamChosen();
        }
    }

    // disable the button
    if (BlueTeamButton)
    {
        BlueTeamButton->SetIsEnabled(false);
        RedTeamButton->SetIsEnabled(true);

    }
    GEngine->AddOnScreenDebugMessage(-1, 8.F, FColor::FromHex("#FFD801"), __FUNCTION__);
}
