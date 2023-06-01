// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAssetManager.h"
#include "Blaster/Input/BlasterGameplayTags.h"

UBlasterAssetManager::UBlasterAssetManager()
{

}

UBlasterAssetManager& UBlasterAssetManager::Get()
{
	check(GEngine);

	UBlasterAssetManager* BlasterAssetManager = Cast<UBlasterAssetManager>(GEngine->AssetManager);
	return *BlasterAssetManager;
}

void UBlasterAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load Native Tags
	FBlasterGameplayTags::InitializeNativeTags();
}