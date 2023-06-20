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
#include "Components/Image.h"


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
                }, 1.0f, false);
        }
        GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UTeamChoice::UpdateScrollBoxes, 1.0f, true);
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
        GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
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
        }
    }
    // Disable the button
    if (RedTeamButton)
    {
        RedTeamButton->SetIsEnabled(false);
        BlueTeamButton->SetIsEnabled(true);
    }
    if (GS)
    {
       // GS->ServerChosenRed(this->GetOwningPlayerState());
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
            BlasterPlayerController->Server_BlueTeamChosen();
        }
    }
    // Disable the button
    if (RedTeamButton)
    {
        BlueTeamButton->SetIsEnabled(false);
        RedTeamButton->SetIsEnabled(true);
    }
    if (GS)
    {
        // GS->ServerChosenRed(this->GetOwningPlayerState());
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
    FLinearColor LightRedColor(1.0f, 0.1f, 0.1f, 1.0f);
    FLinearColor GreenColor(0.0f, 1.0f, 0.0f, 1.0f);

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

            APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();

            if (InGamePlayer == LocalPlayerController->PlayerState)
            {
                TeamChoiceLineWidget->Playername->SetColorAndOpacity(GreenColor);
                TeamChoiceLineWidget->PlayerNameBorder->SetColorAndOpacity(GreenColor);
            }
            else
            {
                TeamChoiceLineWidget->Playername->SetColorAndOpacity(LightRedColor);
                TeamChoiceLineWidget->PlayerNameBorder->SetColorAndOpacity(LightRedColor);
            }

            RedTeamChosenScrollBox->AddChild(TeamChoiceLineWidget);
        }
    }
}

void UTeamChoice::InitBlueTeamScrollBox()
{
    FLinearColor CyanColor(0.0f, 1.0f, 1.0f, 1.0f);
    FLinearColor GreenColor(0.0f, 1.0f, 0.0f, 1.0f);

    ABlasterGameState* GS = Cast<ABlasterGameState>(GetWorld()->GetGameState());
    if (BlueTeamChosenScrollBox && GS)
    {
        // Clear the scroll box
        BlueTeamChosenScrollBox->ClearChildren();

        for (APlayerState* PlayerState : GS->BluePlayersArray)
        {
            if (PlayerState == nullptr) continue;

            APlayerState* InGamePlayer = PlayerState;
            if (InGamePlayer == nullptr) continue;

            TeamChoiceLineWidget = CreateWidget<UTeamChoiceLine>(GetWorld(), TeamChoiceLine);
            if (TeamChoiceLineWidget == nullptr) continue;

            TeamChoiceLineWidget->Playername->SetText(FText::FromString(InGamePlayer->GetPlayerName()));
            APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();

            if (InGamePlayer == LocalPlayerController->PlayerState)
            {
                TeamChoiceLineWidget->Playername->SetColorAndOpacity(GreenColor);
                TeamChoiceLineWidget->PlayerNameBorder->SetColorAndOpacity(GreenColor);
            }
            else
            {
                TeamChoiceLineWidget->Playername->SetColorAndOpacity(CyanColor);
                TeamChoiceLineWidget->PlayerNameBorder->SetColorAndOpacity(CyanColor);
            }

            BlueTeamChosenScrollBox->AddChild(TeamChoiceLineWidget);
        }
    }
}

void UTeamChoice::UpdateScrollBoxes()
{
    InitNoTeamScrollBox();
    InitRedTeamScrollBox();
    InitBlueTeamScrollBox();
    // Implement the logic to update the team scroll boxes as well
}

