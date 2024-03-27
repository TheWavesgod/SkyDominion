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

	UPROPERTY()
	class ASkyPlayerState* SkyPlayerState;

	UPROPERTY()
	class ASkyGameState* SkyGameState;

	UPROPERTY(BlueprintReadOnly)
	float ThrusterRatio = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bAfterBurnerActivated = false;

	UPROPERTY(BlueprintReadOnly)
	float Altitude = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bShowLowAltitudeAlert = false;

	UPROPERTY(BlueprintReadOnly)
	float AngleOfAttack = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float GForce = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWheelRetreated = false;

	UPROPERTY(BlueprintReadOnly)
	float HealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	int AutoCannonBulletAmount = 0;

	UPROPERTY(BlueprintReadOnly)
	FRotator FighterRotation;

	UPROPERTY(BlueprintReadOnly)
	int32 Kill;

	UPROPERTY(BlueprintReadOnly)
	int32 Defeat;

	UPROPERTY(BlueprintReadOnly)
	int Score_RedTeam;

	UPROPERTY(BlueprintReadOnly)
	int Score_BlueTeam;

	UPROPERTY(BlueprintReadOnly)
	int LeftRoundTime;

	UPROPERTY(BlueprintReadOnly)
	FName MissileName;

	UPROPERTY(BlueprintReadOnly)
	int32 MissileNum;

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void ShowBulletRunoutAlert();

	UFUNCTION(BlueprintCallable)
	void ActivateLowAltitudeAlertSound(bool bActivated);

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateParameter();

private:

};
