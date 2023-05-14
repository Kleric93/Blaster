#pragma once

UENUM(BlueprintType)
enum class EGamemode : uint8
{
    FFA UMETA(DisplayName = "Free For All"),
    TDM UMETA(DisplayName = "Team Deathmatch"),
    CTF UMETA(DisplayName = "Capture the Flag"),
    InstaKill UMETA(DisplayName = "InstaKill")
};
