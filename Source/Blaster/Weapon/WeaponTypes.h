#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),

	EWT_NotAWeapon UMETA(DisplayName = "Not A Weapon Sadly"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};
