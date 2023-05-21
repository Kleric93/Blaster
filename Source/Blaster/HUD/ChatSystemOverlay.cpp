// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSystemOverlay.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "ChatBox.h"
#include "Components/ScrollBox.h"

void UChatSystemOverlay::SetChatText(const FString& Text, const FString& PlayerName)
{
    const FString Chat = PlayerName + " : " + Text;

    if (ChatBoxClass)
    {
        OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayer() : OwningPlayer;
        if (OwningPlayer)
        {
            UChatBox* ChatBoxWidget = CreateWidget<UChatBox>(OwningPlayer, ChatBoxClass);
            if (InputScrollBox && ChatBoxWidget && ChatBoxWidget->ChatTextBlock)
            {
                ChatBoxWidget->ChatTextBlock->SetText(FText::FromString(Chat));
                ChatBoxWidget->ChatTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.8f, 1.0f, 1.0f)));
                ChatBoxWidget->ChatTextBlock->SetAutoWrapText(true);
               // InputScrollBox->AddChild(ChatBoxWidget->ChatTextBlock);
                InputScrollBox->AddChild(ChatBoxWidget->ChatTextBlock);
                InputScrollBox->ScrollToEnd();
                InputScrollBox->bAnimateWheelScrolling = true;
            }
        }
    }
}