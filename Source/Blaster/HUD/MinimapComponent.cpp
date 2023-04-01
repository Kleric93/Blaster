

#include "MinimapComponent.h"
#include "Components/ActorComponent.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"



UMinimapComponent::UMinimapComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UMinimapComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UMinimapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}
