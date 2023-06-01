// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
    APlayerState* PlayerState = InPawn->GetPlayerState();

    if (PlayerState != nullptr)
    {
        FString PlayerName = PlayerState->GetPlayerName();
        //UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *PlayerName);
        FString PlayerNameToDisplay = FString::Printf(TEXT("%s"), *PlayerName);
        SetDisplayText(PlayerNameToDisplay);
    }
    else
    {
		FString PlayerNameToDisplay = FString::Printf(TEXT("Training Bot"));
        //UE_LOG(LogTemp, Warning, TEXT("PlayerState is null"));
		return;
    }
}

void UOverheadWidget::ChangeOWColor(ETeam Team)
{
	if (Team == ETeam::ET_NoTeam)
	{
		OverheadWidgetBorder->SetColorAndOpacity(FLinearColor::White);
		DisplayText->SetColorAndOpacity(FLinearColor::White);
	}
	else if (Team == ETeam::ET_RedTeam)
	{
		OverheadWidgetBorder->SetColorAndOpacity(FLinearColor::Red);
		DisplayText->SetColorAndOpacity(FLinearColor::Red);
	}
	else if (Team == ETeam::ET_BlueTeam)
	{
		OverheadWidgetBorder->SetColorAndOpacity(FLinearColor::Blue);
		DisplayText->SetColorAndOpacity(FLinearColor::Blue);
	}
}

void UOverheadWidget::SetTextSize(int32 Size)
{
	if (DisplayText)
	{
		FSlateFontInfo fontInfo = DisplayText->Font;
		fontInfo.Size = Size;
		DisplayText->SetFont(fontInfo);
	}
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}
