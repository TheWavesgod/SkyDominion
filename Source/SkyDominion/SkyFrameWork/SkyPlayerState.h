// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SkyPlayerState.generated.h"

/** Fighter Jet Type */
UENUM(BlueprintType)
enum class EFighterJetType : uint8
{
    E_F15E UMETA(DisplayName = "F15 E"),
    E_Su33 UMETA(DisplayName = "Su 33"),
    E_F18C UMETA(DisplayName = "F18 C"),
	E_Su24 UMETA(DisplayName = "Su 24"),

	E_Max UMETA(DisplayName = "Max")
};


/**
 * 
 */
UCLASS()
class SKYDOMINION_API ASkyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	bool bInRedTeam = true;

	UPROPERTY(Replicated)
	int32 TeamIndex = 0;

	UPROPERTY(Replicated)
	EFighterJetType ChoosedFighterType = EFighterJetType::E_F15E;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerChangeChoosedFighterType(int32 Index);

	void AddKill();
	void AddDefeat();

private:
	UPROPERTY(Replicated)
	int32 Defeat = 0;

public:
	FORCEINLINE int32 GetDefeat() const { return Defeat; }
};
