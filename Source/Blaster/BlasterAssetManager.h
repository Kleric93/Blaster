// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "BlasterAssetManager.generated.h"

/**
 * Manager for the Blaster project
 */
UCLASS()
class BLASTER_API UBlasterAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	UBlasterAssetManager();

	// Returns the AssetManager singleton object.
	static UBlasterAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;

};
