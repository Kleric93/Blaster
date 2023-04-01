/*
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "POIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UPOIComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPOIComponent();

	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void RegisterPointOfInterest(AActor* PointOfInterest);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<AActor*> PointsOfInterest;
};
*/