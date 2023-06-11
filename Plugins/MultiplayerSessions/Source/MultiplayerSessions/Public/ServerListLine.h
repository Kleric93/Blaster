#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemTypes.h"
#include "ServerListLine.generated.h"

class UButton;
class UTextBlock;


UCLASS()
class MULTIPLAYERSESSIONS_API UServerListLine : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
        UTextBlock* ServerIDText;

    UPROPERTY(meta = (BindWidget))
        UTextBlock* HostNameText;

    UPROPERTY(meta = (BindWidget))
        UTextBlock* MatchTypeText;

    UPROPERTY(meta = (BindWidget))
        UTextBlock* CurrentPlayersText;

    UPROPERTY(meta = (BindWidget))
        UTextBlock* MaxPlayersText;

    UPROPERTY(meta = (BindWidget))
        UTextBlock* ServerPingText;

};
