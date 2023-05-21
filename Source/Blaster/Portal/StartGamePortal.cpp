

#include "StartGamePortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"

AStartGamePortal::AStartGamePortal()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and set up the static mesh component
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = PortalMesh; // Set the static mesh component as the root component

	// Create and set up the collision box component
	CollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
	CollisionBoxComponent->SetupAttachment(PortalMesh); // Attach the collision box component to the static mesh component
	CollisionBoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // Set the relative location of the collision box component if needed
}

void AStartGamePortal::BeginPlay()
{
	Super::BeginPlay();

	CollisionBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStartGamePortal::OnOverlapBegin);
}

void AStartGamePortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStartGamePortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (OverlappedComp == CollisionBoxComponent)
	{
		UGameInstance* GameInstance = GetGameInstance();

		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		UE_LOG(LogTemp, Warning, TEXT("Overlap detected with %s"), *OtherActor->GetName());

		UWorld* World = GetWorld();
		if (World)
		{
			AGameModeBase* GameMode = World->GetAuthGameMode();
			if (GameMode && HasAuthority()) // Check if this actor has authority
			{
				UE_LOG(LogTemp, Warning, TEXT("Remote Role: %d, Authority: %s"), static_cast<int32>(GetRemoteRole()), HasAuthority() ? TEXT("true") : TEXT("false"));

				GameMode->bUseSeamlessTravel = true;

				FString MatchType = Subsystem->DesiredMatchType;
				FString TravelURL;

				if (MatchType == "FreeForAll")
				{
					World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
				}
				else if (MatchType == "Teams")
				{
					World->ServerTravel(FString("/Game/Maps/Teams?listen"));
				}
				else if (MatchType == "CaptureTheFlag")
				{
					World->ServerTravel(FString("/Game/Maps/CaptureTheFlag?listen"));
				}
				else if (MatchType == "InstaKill")
				{
					World->ServerTravel(FString("/Game/Maps/InstaKillMap?listen"));
				}

				UE_LOG(LogTemp, Warning, TEXT("Attempting server travel to: %s"), *TravelURL);
				bool bTravelSuccessful = World->ServerTravel(TravelURL);

				if (!bTravelSuccessful)
				{
					UE_LOG(LogTemp, Error, TEXT("Server travel failed!"));
				}
			}
			else
			{
				return;
			}
		}
	}
}



