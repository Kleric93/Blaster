// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Blaster.h"
#include "GameFramework/PlayerStart.h"
#include "Sound/SoundCue.h"
#include "Blaster/PlayerStates/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blaster/KillBox.h"
#include "Blaster/HUD/VotingSyastem.h"
#include "Blaster/HUD/VotingSyastem.h"
#include "Blaster/BlasterUserSettings.h"
#include "Components/AudioComponent.h"


namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
    bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
    Super::BeginPlay();

    LevelStartingTime = GetWorld()->GetTimeSeconds();

    if (Settings == nullptr)
    {
        Settings = Cast<UBlasterUserSettings>(GEngine->GameUserSettings);
    }

    MatchTime = Settings->GetGameTime();

    BlasterGameState = GetGameState<ABlasterGameState>();
}

void ABlasterGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // Start the soundtrack when a player logs in and the match state is InProgress
    if (GetMatchState() == MatchState::InProgress)
    {
        ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(NewPlayer);
        if (PlayerController && !PlayerController->bSoundtrackHasStarted) // We check that the soundtrack hasn't started for this player yet.
        {
            PlayerController->Client_BeginSoundtrack();
            PlayerController->bSoundtrackHasStarted = true;
            PlayerController->ServerClientJoined(NewPlayer->PlayerState->GetPlayerName());
        }
    }
}

void ABlasterGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    float NewTimeElapsed = BlasterGameState->GetTimeElapsed() + DeltaTime;

    if (MatchState != MatchState::Cooldown)
    {
        BlasterGameState->SetTimeElapsed(NewTimeElapsed);

    }

    if (MatchState == MatchState::WaitingToStart)
    {
        CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

        if (CountdownTime <= 0.f)
        {
            StartMatch();
        }
    }
    else if (MatchState == MatchState::InProgress)
    {

        float RedScore = BlasterGameState->RedTeamScore;
        float BlueScore = BlasterGameState->BlueTeamScore;
        float TopScoringPlayerScore = BlasterGameState->TopScore;

        if (!bTeamsMatch && !bCaptureTheFlagMatch)
        {
            if (TopScoringPlayerScore >= Settings->GetMaxScore())
            {
                SetMatchState(MatchState::Cooldown);
                BlasterGameState->SetHasMatchEndedAbruptly(true);
            }
        }
        else
        {
            if (RedScore >= Settings->GetMaxScore() || BlueScore >= Settings->GetMaxScore())
            {
                SetMatchState(MatchState::Cooldown);
                BlasterGameState->SetHasMatchEndedAbruptly(true);
            }
        }
        CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("MaxScore IS %f"), Settings->GetMaxScore()));

        if (CountdownTime <= 0.f)
        {
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if (MatchState == MatchState::Cooldown)
    {
        if (BlasterGameState->HasMatchEndedAbruptly())
        {
            CountdownTime = CooldownTime + WarmupTime  + BlasterGameState->GetTimeElapsed() - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        }
        else
        {
            CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        }

        if (CountdownTime <= 0.f)
        {
            if (BlasterGameState == nullptr) return;

            FString mapName = BlasterGameState->CompareVotesAndLog();

            ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
            if (BlasterPlayerController && BlasterPlayerController->VotingSystem)
            {
                BlasterPlayerController->VotingSystem->MenuTeardown();
            }

            if (!mapName.IsEmpty() && HasAuthority())
            {
                GetWorld()->ServerTravel(mapName + "?listen", GetTravelType());
            }
            else
            {
                RestartGame();
            }
        }
    }
}

void ABlasterGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();


    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if (BlasterPlayer)
        {
            BlasterPlayer->OnMatchStateSet(MatchState, bTeamsMatch, bCaptureTheFlagMatch);
        }
    }

    TArray<ABlasterPlayerState*> BlasterPlayerStateArray;
    for (ABlasterPlayerState* BlasterPlayerState : BlasterPlayerStateArray)
    {
        if (!bTeamsMatch && !bCaptureTheFlagMatch)
        {
            BlasterPlayerState->SetTeam(ETeam::ET_NoTeam);
        }
    }
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
    return BaseDamage;
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
    //if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
   // if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
    ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;


    if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
    {
        TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
        for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
        {
            PlayersCurrentlyInTheLead.Add(LeadPlayer);
        }

        AttackerPlayerState->AddToScore(1.f);
        AttackerPlayerState->EventKilledPlayer();
        BlasterGameState->UpdateTopScore(AttackerPlayerState);
        if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
        {
            ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
            if (Leader)
            {
                Leader->MulticastGainedTheLead();
            }
        }

        for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
        {
            if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
            {
                ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
                if (Loser)
                {
                    Loser->MulticastLostTheLead();
                }
            }
        }
    }

    if (VictimPlayerState)
    {
        VictimPlayerState->AddToDefeats(1);
    }

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
        {
            BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
        }
    }
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Reset();
        ElimmedCharacter->Destroy();
    }

    if (ElimmedController)
    {
      // if (MatchState == MatchState::Cooldown) return;

        // Find all available player starts
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

        // Define maximum number of attempts to find a suitable spawn location
        const int32 MaxAttempts = 10;

        // Loop through all player starts for a fixed number of attempts and select the first one that has no overlapping actors
        for (int32 i = 0; i < MaxAttempts; i++)
        {
            // Select a random player start
            int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
            FVector StartLocation = PlayerStarts[Selection]->GetActorLocation();

            // Check for nearby players
            TArray<FOverlapResult> Overlaps;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(ElimmedCharacter);
            QueryParams.bTraceComplex = true;
            GetWorld()->OverlapMultiByObjectType(
                Overlaps,
                StartLocation,
                FQuat::Identity,
                FCollisionObjectQueryParams(ECC_SkeletalMesh),
                FCollisionShape::MakeSphere(NearbyRadius),
                QueryParams);

            //UE_LOG(LogTemp, Warning, TEXT("Number of overlapping actors found: %d"), Overlaps.Num());

            // If no nearby players are found, respawn at the current player start and exit the loop
            if (Overlaps.Num() == 0)
            {
                RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
               // UE_LOG(LogTemp, Warning, TEXT("Respawn Successful for: %s"), *ElimmedController->GetName());
                return;
            }

            ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ElimmedController->GetPawn());

            if (MatchState == MatchState::Cooldown && BlasterCharacter)
            {
                BlasterCharacter->bDisableGameplay = true;
                BlasterCharacter->DisableInput(BlasterCharacter->GetPlayerState()->GetPlayerController());
            }
        }

        // If no suitable spawn location is found, use a default location or return an error
        //UE_LOG(LogTemp, Warning, TEXT("No suitable spawn location found for player %s"), *ElimmedController->GetName());
        //FRotator DefaultRotation = FRotator(0.0f, 0.0f, 0.0f);
        //RestartPlayerAtTransform(ElimmedController, FTransform(DefaultRotation, DefaultLocation));
        int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);

    }
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
    if (PlayerLeaving == nullptr) return;
    if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
    {
        BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);

    }
    ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
    if (CharacterLeaving)
    {
        CharacterLeaving->Elim(true);
        BlasterGameState->Multicast_RemovePlayerLeft(PlayerLeaving);
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PlayerLeftGame: PlayerLeaving is %s"), *PlayerLeaving->GetName()));
    }
}

void ABlasterGameMode::SendChat(const FString& Text, const FString& PlayerName)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(*It);
        if (BlasterPlayerController)
        {
            BlasterPlayerController->ClientSetText(Text, PlayerName);
        }
    }
}