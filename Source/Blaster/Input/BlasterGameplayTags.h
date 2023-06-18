// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * MyGameplayTags
 *
 *	Singleton containing native gameplay tags.
 */
struct FBlasterGameplayTags
{
public:

	static const FBlasterGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	//Input Tags
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Equip;
	FGameplayTag InputTag_Swap;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_Prone;
	FGameplayTag InputTag_Aim;
	FGameplayTag InputTag_Fire;
	FGameplayTag InputTag_Reload;
	FGameplayTag InputTag_Grenade;
	FGameplayTag InputTag_PhantomStride;


protected:

	//Registers all of the tags with the GameplayTags Manager
	void AddAllTags(UGameplayTagsManager& Manager);

	//Helper function used by AddAllTags to register a single tag with the GameplayTags Manager
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	static FBlasterGameplayTags GameplayTags;
};