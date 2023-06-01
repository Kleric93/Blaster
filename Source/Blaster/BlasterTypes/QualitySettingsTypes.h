#pragma once

UENUM(BlueprintType)
enum class EGraphicalSettings : uint8 {
	EGS_View_Distance		UMETA(DisplayName = "View_Distance"),
	EGS_Anti_Aliasing		UMETA(DisplayName = "Anti_Aliasing"),
	EGS_Post_Processing		UMETA(DisplayName = "Post_Processing"),
	EGS_Shadows				UMETA(DisplayName = "Shadows"),
	EGS_Global_Illumination	UMETA(DisplayName = "Global_Illumination"),
	EGS_Reflection			UMETA(DisplayName = "Reflection"),
	EGS_Textures			UMETA(DisplayName = "Textures"),
	EGS_Effects				UMETA(DisplayName = "Effects"),
	EGS_Foliage				UMETA(DisplayName = "Foliage"),
	EGS_Shading				UMETA(DisplayName = "Shading"),
	EGS_MAX					UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EGraphicalSettings, EGraphicalSettings::EGS_MAX);


UENUM(BlueprintType)
enum class ESettingsLevel : uint8 {
	ESL_Low		UMETA(DisplayName = "Low"),
	ESL_Mid		UMETA(DisplayName = "Mid"),
	ESL_High	UMETA(DisplayName = "High"),
	ESL_Epic	UMETA(DisplayName = "Epic"),
	ESL_Cinema	UMETA(DisplayName = "Cinema"),
	ESL_MAX		UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ESettingsLevel, ESettingsLevel::ESL_MAX);


//Settings Level Text
#define SLT_Low		FText::FromString("Low")
#define SLT_Mid		FText::FromString("Mid")
#define SLT_High	FText::FromString("High")
#define SLT_Epic	FText::FromString("Epic")
#define SLT_Cinema	FText::FromString("Cinema")


//SPT - Settings parameter text

#define SPT_View_Distance		FText::FromString("View_Distance")
#define SPT_Anti_Aliasing		FText::FromString("Anti_Aliasing")
#define SPT_Post_Processing		FText::FromString("Post_Processing")
#define SPT_Shadows				FText::FromString("Shadows")
#define SPT_Global_Illumination	FText::FromString("Global_Illumination")
#define SPT_Reflection			FText::FromString("Reflection")
#define SPT_Textures			FText::FromString("Textures")
#define SPT_Effects				FText::FromString("Effects")
#define SPT_Foliage				FText::FromString("Foliage")
#define SPT_Shading				FText::FromString("Shading")