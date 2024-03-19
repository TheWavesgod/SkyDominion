// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpectatorOverlay.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class SKYDOMINION_API USpectatorOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float RespawnCoolingDownTime = 10.0f;

	UPROPERTY(BlueprintReadOnly)
	int32 Kills;

	UPROPERTY(BlueprintReadOnly)
	int32 Deaths;

	UPROPERTY(BlueprintReadOnly)
	int32 RespawnCD;

protected:
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	UTextBlock* TextBlock_RespawnHint;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateInfo(float InDeltaTime);

	float RespawnCoolingDownTimer;

	class ASkyPlayerState* SkyPlayerState;

public:
	void Init();
};
