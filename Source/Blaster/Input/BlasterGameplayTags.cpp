

#include "BlasterGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

FBlasterGameplayTags FBlasterGameplayTags::GameplayTags;

void FBlasterGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(GameplayTagsManager);

	GameplayTagsManager.DoneAddingNativeTags();
}

void FBlasterGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	AddTag(InputTag_Jump, "InputTag.Jump", "Jump input");
	AddTag(InputTag_Equip, "InputTag.Equip", "Equip input.");
	AddTag(InputTag_Swap, "InputTag.Swap", "Swap input.");
	AddTag(InputTag_Crouch, "InputTag.Crouch", "Crouch input.");
	AddTag(InputTag_Prone, "InputTag.Prone", "Prone input.");
	AddTag(InputTag_Aim, "InputTag.Aim", "Aim input.");
	AddTag(InputTag_Fire, "InputTag.Fire", "Fire input.");
	AddTag(InputTag_Reload, "InputTag.Reload", "Reload input.");
	AddTag(InputTag_Grenade, "InputTag.Grenade", "Grenade input.");
	AddTag(InputTag_PhantomStride, "InputTag.PhantomStride", "PhantomStride input.");
	AddTag(InputTag_Melee, "InputTag.Melee", "Melee input.");
}

void FBlasterGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
