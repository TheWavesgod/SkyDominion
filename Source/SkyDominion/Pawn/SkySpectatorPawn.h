// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "SkySpectatorPawn.generated.h"

UCLASS()
class SKYDOMINION_API ASkySpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ASkySpectatorPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void RespawnRequestPressed();

public:	

};
