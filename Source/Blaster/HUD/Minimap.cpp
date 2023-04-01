// Fill out your copyright notice in the Description page of Project Settings.
/*

#include "Minimap.h"
#include "MinimapComponent.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "POIComponent.h"

UMinimap::UMinimap()
{
	this->ArrowTexture = nullptr;
	this->RadarStartLocation = FVector2D(10.f, 10.f);
	this->RadarSize = 200.f;
	this->DegreeStep = 0.25f;
	this->MaxAngleStep = 360.f;
	this->RadarDistanceScale = 25.f;
	this->SphereHeight = 200.f;
	this->SphereRadius = 3000.f;
	this->EnemyRectSize = 3.f;
	this->R = 1.f;
	this->G = 0.f;
	this->B = 0.f;

	this->ArrowTexture = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Textures/arrow.arrow'"));
}

void ARadar2D::DrawHUD()
{
	this->DrawRadar(FLinearColor::Gray);

	this->DrawLocalPlayerOnRadar();

	this->GetRaycastedActorsFromWorld();

	this->DrawPlayersOnMiniMap();
}

void ARadar2D::DrawPlayersOnMiniMap()
{
	FVector2D RadarCenter = GetRadarCenter();

	for (AActor* It : OnWorldActors)
	{
		FVector2D Location2D = WorldToScreen2D(It);

		//Clamp positions in between the minimap size so they are not out of bounds
		float NewX = FMath::Clamp<float>(Location2D.X, -this->Size / 2, this->Size / 2 - EnemyRectSize);
		float NewY = FMath::Clamp<float>(Location2D.Y, -this->Size / 2, this->Size / 2 - EnemyRectSize);

		FVector MyLocation = GetCurrentActorLocation();
		FVector EnemyLocation = It->GetActorLocation();

		float Distance = FVector::Distance(MyLocation, EnemyLocation);

		//fade color based on distance
		//im the same level enemy it's red with it's fade?
		this->EnemyColor = FLinearColor(this->R, this->G, this->B, this->Alpha / Distance * 4);
		this->DrawRect(this->EnemyColor, RadarCenter.X + NewX, RadarCenter.Y + NewY, EnemyRectSize, EnemyRectSize);

#pragma region OLD STUFF OMG
		/*float X1 = (-GetCurrentActorLocation().X * .075f) * FMath::Cos(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw)
		- (-GetCurrentActorLocation().Y * .075f) * FMath::Sin(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw);
		float Y1 = (-GetCurrentActorLocation().X * .075f) * FMath::Sin(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw)
		+ (-GetCurrentActorLocation().Y * .075f) * FMath::Cos(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw);
		int OffYR = ((GetRadarCenter().Y * 0.3741) - ((GetRadarCenter().Y * 0.3741) * 2.05)), OffXR = 0;
		float shiftX = (((GetRadarCenter().X) + OffXR) - X1);
		float shiftY = (((GetRadarCenter().Y) + OffYR) - Y1);
		float X2 = ((-convertedLocation.X * .075f) * FMath::Cos(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw)
		- (-convertedLocation.Y * .075f) * FMath::Sin(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw)) + shiftX;
		float Y2 = ((-convertedLocation.X * .075f) * FMath::Sin(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw)
		+ (-convertedLocation.Y * .075f) * FMath::Cos(this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Yaw)) + shiftY;
		//UE_LOG(LogTemp, Warning, TEXT("x1: %f y1: %f"), X2, Y2);
		DrawRect(FLinearColor::Red, (RadarCenter.X + X2 ), (RadarCenter.Y + Y2), DrawPixelSize, DrawPixelSize);
#pragma endregion OLD STUFF OMG
	}
}

void ARadar2D::DrawLocalPlayerOnRadar()
{
	if (!this->ArrowTexture)
		return;

	float TextureWidth = this->ArrowTexture->GetSurfaceWidth();
	float TextureHeight = this->ArrowTexture->GetSurfaceHeight();

	this->DrawTextureSimple(this->ArrowTexture, GetRadarCenter().X - TextureWidth * this->FixedTextureScale / 2, GetRadarCenter().Y - TextureHeight * this->FixedTextureScale / 2, this->FixedTextureScale);
}

void ARadar2D::GetRaycastedActorsFromWorld()
{
	APawn* LocalPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (!LocalPlayer)
		return;

	TArray<FHitResult> HitResults;
	FVector EndLocation = LocalPlayer->GetActorLocation();
	EndLocation.Z += SphereHeight;

	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(SphereRadius);

	this->GetWorld()->SweepMultiByChannel(HitResults, GetCurrentActorLocation(),
		EndLocation, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, CollisionShape);

	for (FHitResult It : HitResults)
	{
		AActor* CurrentActor = It.GetActor();

		if (CurrentActor == LocalPlayer)
			continue;

		if (CurrentActor == nullptr)
			continue;

		OnWorldActors.Add(CurrentActor);
	}
}

void ARadar2D::DrawRadar(FLinearColor Color)
{
	this->PosX = this->RadarStartLocation.X;
	this->PosY = this->RadarStartLocation.Y;
	this->Size = this->RadarSize;

	//square
	DrawRect(Color, PosX, PosY, 1, Size);
	DrawRect(Color, PosX, PosY, Size, 1);
	DrawRect(Color, PosX, PosY + Size, Size, 1);
	DrawRect(Color, PosX + Size, PosY, 1, Size + 1);

	//vertical line
	DrawRect(Color, PosX, PosY + Size / 2, Size, 1);
	//horizontal line
	DrawRect(Color, PosX + Size / 2, PosY, 1, Size + 1);

	//background
	DrawRect(FLinearColor(0, 0, 0, 0.3f), PosX, PosY, Size, Size);
}

FVector2D ARadar2D::GetRadarCenter()
{
	if (Canvas)
		return FVector2D(this->PosX + this->Size / 2, this->PosY + this->Size / 2);
	else
		return FVector2D(0, 0);
}

FVector ARadar2D::GetCurrentActorLocation()
{
	return this->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
}

FVector2D ARadar2D::WorldToScreen2D(AActor* Actor)
{
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (Player && Actor) //check if both are not bad pointers
	{
		FVector ActorPosition3D = Player->GetTransform().InverseTransformPosition(Actor->GetActorLocation());
		ActorPosition3D = FRotator(0.f, -90.f, 0.f).RotateVector(ActorPosition3D);
		ActorPosition3D /= RadarDistanceScale;

		return FVector2D(ActorPosition3D);
	}
	return FVector2D(0, 0);
}

void ARadar2D::DrawCircle2D(float ScreenX, float ScreenY, FLinearColor Color, float Tickness)
{
	for (float Step = 0.f; Step < MaxAngleStep; Step += DegreeStep)
	{
		float StepX = FMath::Cos(Step);
		float StepY = FMath::Sin(Step);

		DrawLine(ScreenX, ScreenY, ScreenX + StepX, ScreenY + StepY, Color, Tickness);
	}
}*/
