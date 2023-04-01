
  /*
#include "POIComponent.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"

UPOIComponent::UPOIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UPOIComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UPOIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  
    // Update the position of each registered PointOfInterest in the Minimap
   // This assumes that the PointOfInterest actor has a valid mesh component
    for (AActor* PointOfInterest : PointsOfInterest)
    {
        FVector ActorLocation = PointOfInterest->GetActorLocation();
        FVector LocalPosition = GetComponentTransform().InverseTransformPosition(ActorLocation);

        // Set the X and Y positions of the PointOfInterest widget based on its position relative to the Minimap component
        float X = (LocalPosition.Y / 1000.0f) * 150.0f + 150.0f;
        float Y = (LocalPosition.X / 1000.0f) * 150.0f + 150.0f;

        // Update the position of the PointOfInterest widget
        // This assumes that the PointOfInterest widget has a valid reference to its parent Minimap widget
        UWidget* Widget = PointOfInterest->FindComponentByClass<UWidget>();
        if (Widget)
        {
            UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
            if (CanvasSlot)
            {
                CanvasSlot->SetPosition(FVector2D(X, Y));
            }
        }
    }
}

void UPOIComponent::RegisterPointOfInterest(AActor* PointOfInterest)
{
    //PointsOfInterest.AddUnique(PointOfInterest);
}
*/