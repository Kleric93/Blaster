#pragma once

UENUM(BlueprintType)
enum class EGrenadeType : uint8 {
	EGT_None UMETA(DisplayName = "None"), 
	EGT_Seismic UMETA(DisplayName = "Frag"), 
	EGT_Smoke UMETA(DisplayName = "Smoke")
};