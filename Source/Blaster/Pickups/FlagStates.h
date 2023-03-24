#pragma once

UENUM(BlueprintType)
enum class EFlagState : uint8
{
	EFS_Initial UMETA(DisplayName = "Initial State"),
	EFS_Equipped UMETA(DisplayName = "Equipped"),
	EFS_Dropped UMETA(DisplayName = "Dropped"),

	EFS_MAX UMETA(DisplayName = "DefaultMAX"),

};