// Fill out your copyright notice in the Description page of Project Settings.
/*
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Minimap.generated.h"

// Forward declarations
class UTexture2D;

UCLASS()
class BLASTER_API UMinimap : public UUserWidget
{
	GENERATED_BODY()

private:
	UMinimap();

	//override functions inherited
	virtual void DrawHUD() override;

	//Radar related functions & vars
	void DrawPlayersOnMiniMap();
	void DrawLocalPlayerOnRadar();
	void GetRaycastedActorsFromWorld();
	void DrawRadar(FLinearColor Color = FLinearColor::Yellow);
	FVector2D GetRadarCenter();
	float PosX, PosY, Size;
	TArray<FHitResult*> Results;
	FVector GetCurrentActorLocation();
	FVector2D WorldToScreen2D(AActor* ActorToPlace);
	TArray<AActor*> OnWorldActors;
	const float Alpha = 1.f;
	FLinearColor EnemyColor;

	//custom functions
	void DrawCircle2D(float ScreenX, float ScreenY, FLinearColor Color, float Tickness = 0.f);

	//texture related
	UTexture2D* ArrowTexture;
	const float FixedTextureScale = 0.02f;

public:
	UPROPERTY(EditAnywhere)
		FVector2D RadarStartLocation;

	UPROPERTY(EditAnywhere)
		float RadarSize;

	UPROPERTY(EditAnywhere)
		float DegreeStep;

	UPROPERTY(EditAnywhere)
		float MaxAngleStep;

	UPROPERTY(EditAnywhere, Category = MiniMap2D)
		float RadarDistanceScale;

	UPROPERTY(EditAnywhere, Category = MiniMap2D)
		float SphereHeight;

	UPROPERTY(EditAnywhere, Category = MiniMap2D)
		float SphereRadius;

	UPROPERTY(EditAnywhere, Category = MiniMap2D)
		float EnemyRectSize;

	UPROPERTY(EditAnywhere, Category = MiniMap2D, DisplayName = "Enemy Color Red")
		float R;

	UPROPERTY(EditAnywhere, Category = MiniMap2D, DisplayName = "Enemy Color Green")
		float G;

	UPROPERTY(EditAnywhere, Category = MiniMap2D, DisplayName = "Enemy Color Blue")
		float B;
};*/