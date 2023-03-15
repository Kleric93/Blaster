// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncement.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UElimAnnouncement::SetElimAnnouncementText(FString AttackerName, FString VictimName)
{
	FString ElimAnnouncementText = FString::Printf(TEXT("%s Killed %s with"), *AttackerName, *VictimName);
	
	if (AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ElimAnnouncementText));
	}
}

void UElimAnnouncement::SetElimAnnouncementWeaponIcon(UTexture2D* ElimWeaponIcon)
{
	if (WeaponIcon)
	{
		// Set the provided texture as the widget's brush
		WeaponIcon->SetBrushFromTexture(ElimWeaponIcon);
	}
}
