
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartGamePortal.generated.h"

UCLASS()
class BLASTER_API AStartGamePortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AStartGamePortal();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PortalMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBoxComponent;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
