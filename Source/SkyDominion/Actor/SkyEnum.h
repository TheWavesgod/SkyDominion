#pragma once

UENUM()
enum class ERadarMode : uint8
{
	RWS UMETA(DisplayName = "RWS"),
	VT UMETA(DisplayName = "VT"),
	STT UMETA(DisplayName = "STT"),

	Max UMETA(DisplayName = "Max")
}; 

UENUM()
enum class ETargetMarkState : uint8
{
	Lost UMETA(DisplayName = "Lost"),
	TeamMate UMETA(DisplayName = "Team Mate"),
	RWSEnemy UMETA(DisplayName = "RWSEnemy"),
	Enemy UMETA(DisplayName = "Enemy"),
	Locking UMETA(DisplayName = "Locking"),
	Locked  UMETA(DisplayName = "Locked"),

	TargetMarkStateMax UMETA(DisplayName = "TargetMarkStateMax")
};