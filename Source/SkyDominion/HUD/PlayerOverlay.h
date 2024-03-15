// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	class AFighter* fighter;

	UPROPERTY(BlueprintReadOnly)
	float ThrusterRatio = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bAfterBurnerActivated = false;

	UPROPERTY(BlueprintReadOnly)
	float Altitude = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float AngleOfAttack = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float GForce = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWheelRetreated = false;

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateParameter();

private:

};
