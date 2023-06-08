// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamChoice.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TeamChoiceLine.h"

void UTeamChoice::WidgetSetup()
{
    AddToViewport(5);
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    ABlasterGameState* GS = Cast<ABlasterGameState>(GetWorld()->GetGameState());
    ABlasterPlayerState* PS = Cast<ABlasterPlayerState>(GetOwningPlayerState());
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
        if (PS)
        {
          
        }
        if (GS)
        {
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                {
                    InitNoTeamScrollBox();
                }, 3.0f, false);
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
    ABlasterGameState* GS = Cast<ABlasterGameState>(GetWorld()->GetGameState());

    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->Server_RedTeamChosen();
            BlasterPlayerController->Server_UpdateScrollBoxes();
            /*
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
                BlasterPlayerController->Server_RedTeamChosen();
                }, 2.0f, false);*/
        }
        if (GS && GS->BluePlayersArray.Contains(this->GetOwningPlayerState()))
        {
            GS->BluePlayersArray.Remove(this->GetOwningPlayerState());
            GS->RedPlayersArray.Add(this->GetOwningPlayerState());
            GS->ServerChosenRed(this->GetOwningPlayerState());
            InitRedTeamScrollBox();
        }
        else if (GS->PendingChoicePlayerArray.Contains(this->GetOwningPlayerState()))
        {
            GS->PendingChoicePlayerArray.Remove(this->GetOwningPlayerState());
            GS->RedPlayersArray.Add(this->GetOwningPlayerState());
        }
    }

    // Update the scroll boxes
    UpdateScrollBoxes();

    // Disable the button
    if (RedTeamButton)
    {
        RedTeamButton->SetIsEnabled(false);
        BlueTeamButton->SetIsEnabled(true);
    }
}

void UTeamChoice::OnBlueButtonClicked()
{
    ABlasterGameState* GS = Cast<ABlasterGameState>(GetWorld()->GetGameState());

    if (PlayerController)
    {
        BlasterPlayerController = Cast<ABlasterPlayerController>(PlayerController);
        if (BlasterPlayerController)
        {
            // Add a delay before calling the server RPC function
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
                BlasterPlayerController->Server_BlueTeamChosen();
                }, 2.0f, false);
        }
        if (GS && GS->RedPlayersArray.Contains(this->GetOwningPlayerState()))
        {
            GS->RedPlayersArray.Remove(this->GetOwningPlayerState());
            GS->BluePlayersArray.Add(this->GetOwningPlayerState());
        }
        else if (GS->PendingChoicePlayerArray.Contains(this->GetOwningPlayerState()))
        {
            GS->PendingChoicePlayerArray.Remove(this->GetOwningPlayerState());
            GS->BluePlayersArray.Add(this->GetOwningPlayerState());
        }
    }

    // Update the scroll boxes
    UpdateScrollBoxes();

    // Disable the button
    if (BlueTeamButton)
    {
        BlueTeamButton->SetIsEnabled(false);
        RedTeamButton->SetIsEnabled(true);
    }
}


void UTeamChoice::InitNoTeamScrollBox()
{
    ABlasterGameState* GS = Cast<ABlasterGameState>(GetWorld()->GetGameState());
    if (NoTeamScrollBox && GS)
    {
        // Clear the scroll box
        NoTeamScrollBox->ClearChildren();

        for (APlayerState* PlayerState : GS->PendingChoicePlayerArray)
        {
            if (PlayerState == nullptr) continue;

            APlayerState* InGamePlayer = PlayerState;
            if (InGamePlayer == nullptr) continue;

            TeamChoiceLineWidget = CreateWidget<UTeamChoiceLine>(GetWorld(), TeamChoiceLine);
            if (TeamChoiceLineWidget == nullptr) continue;

            TeamChoiceLineWidget->Playername->SetText(FText::FromString(InGamePlayer->GetPlayerName()));

            NoTeamScrollBox->AddChild(TeamChoiceLineWidget);
        }
    }
}

void UTeamChoice::InitRedTeamScrollBox()
{
    ABlasterGameState* GS = Cast<ABlasterGameState>(GetWorld()->GetGameState());
    if (RedTeamChosenScrollBox && GS)
    {
        // Clear the scroll box
        RedTeamChosenScrollBox->ClearChildren();

        for (APlayerState* PlayerState : GS->RedPlayersArray)
        {
            if (PlayerState == nullptr) continue;

            APlayerState* InGamePlayer = PlayerState;
            if (InGamePlayer == nullptr) continue;

            TeamChoiceLineWidget = CreateWidget<UTeamChoiceLine>(GetWorld(), TeamChoiceLine);
            if (TeamChoiceLineWidget == nullptr) continue;

            TeamChoiceLineWidget->Playername->SetText(FText::FromString(InGamePlayer->GetPlayerName()));

            RedTeamChosenScrollBox->AddChild(TeamChoiceLineWidget);
        }
    }
}

void UTeamChoice::UpdateScrollBoxes()
{
    InitNoTeamScrollBox();
    // Implement the logic to update the team scroll boxes as well
}

