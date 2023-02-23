#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),

	EWT_M4AZ UMETA(DisplayName = "M4AZapper"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};
