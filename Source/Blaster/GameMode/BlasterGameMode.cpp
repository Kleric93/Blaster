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
}

void ABlasterGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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
        CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if (MatchState == MatchState::Cooldown)
    {
        CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            RestartGame();
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
            ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(BlasterPlayer->PlayerState);
            if (!bTeamsMatch && !bCaptureTheFlagMatch)
            {
                BPState->SetTeam(ETeam::ET_NoTeam);
            }
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

    ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

    if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
    {
        TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
        for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
        {
            PlayersCurrentlyInTheLead.Add(LeadPlayer);
        }

        AttackerPlayerState->AddToScore(1.f);
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
            }
        }

        // If no suitable spawn location is found, use a default location or return an error
        UE_LOG(LogTemp, Warning, TEXT("No suitable spawn location found for player %s"), *ElimmedController->GetName());
        FRotator DefaultRotation = FRotator(0.0f, 0.0f, 0.0f);
        RestartPlayerAtTransform(ElimmedController, FTransform(DefaultRotation, DefaultLocation));
    }
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
    if (PlayerLeaving == nullptr) return;
    ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
    if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
    {
        BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);

    }
    ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
    if (CharacterLeaving)
    {
        CharacterLeaving->Elim(true);
        BlasterGameState->Multicast_RemovePlayerLeft(PlayerLeaving);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PlayerLeftGame: PlayerLeaving is %s"), *PlayerLeaving->GetName()));
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