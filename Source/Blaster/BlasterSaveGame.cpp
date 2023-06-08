// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterSaveGame.h"

void UBlasterSaveGame::AddPlayerMapping(FName Name, FKey Key)
{
	PlayerMappings.Add(Name, Key);
}
