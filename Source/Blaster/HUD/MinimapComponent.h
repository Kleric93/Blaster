// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneComponent.h" // Added this line
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MinimapComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UMinimapComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMinimapComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



protected:
	virtual void BeginPlay() override;

private:
};
