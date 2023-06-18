#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_WHITE 248
#define CUSTOM_DEPTH_YELLOW 249
#define CUSTOM_DEPTH_LIGHTBLUE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_BROWN 252
#define CUSTOM_DEPTH_PINK 253
#define CUSTOM_DEPTH_RED 254
#define CUSTOM_DEPTH_GREEN 255



UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),

	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),

	EWT_Pistol UMETA(DisplayName = "Pistol"),

	EWT_SMG UMETA(DisplayName = "SMG"),

	EWT_M4AZ UMETA(DisplayName = "M4AZapper"),

	EWT_Shotgun UMETA(DisplayName = "Shotgun"),

	EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),

	EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),

	EWT_PhantomBlade UMETA(DisplayName = "Phantom Blade"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};
