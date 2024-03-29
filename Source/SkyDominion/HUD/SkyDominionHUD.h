// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SkyDominionHUD.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API ASkyDominionHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Overlay Settings")
	TSubclassOf<class UUserWidget> PlayerOverlayClass;

	UPROPERTY(EditAnywhere, Category = "Overlay Settings")
	TSubclassOf<class UUserWidget> SpectatorOverlayClass;

	UPROPERTY(EditAnywhere, Category = "Overlay Settings")
	TSubclassOf<class UUserWidget> PauseMenuClass;

	UPROPERTY()
	class UPlayerOverlay* PlayerOverlay;

	UPROPERTY()
	class USpectatorOverlay* SpectatorOverlay;

	UPROPERTY()
	class UPauseMenu* PauseMenu;

	UPROPERTY(EditAnywhere, Category = "Cross Hair")
	UTexture2D* Crosshair;

	UPROPERTY(EditAnywhere, Category = "Radar HUD")
	UTexture2D* VTScanLine;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	UTexture2D* MarkPointer;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	FLinearColor TeamateColor;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	FLinearColor EnemyColor;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	FLinearColor LockColor;

	class AFighter* OwnerFighter;

protected:
	virtual void BeginPlay() override;

	void CheckOwnerFighterStateAndPlayerOverlay();

	void DrawCrosshair();

	void DrawTargetMarkPointer();
	
	void GetMarkPointerDrawPos(FVector2D& DrawPos, float& RotDegree, float RelativeX, float RelativeY, FVector2D ScreenCenter, FVector2D TextureSize);

	void DrawRadarVTScanRange();

public:
	void AddPlayerOverlay();
	void SetPlayerOverlayVisibility(bool bIsVisible);

	void AddSpectatorOverlay();
	void RemoveSpectatorOverlay();

	void AddPauseMenu();
	void RemovePauseMenu();
};
