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

	UPROPERTY(EditAnywhere, Category = "Overlay Settings")
	TSubclassOf<class UUserWidget> KillMessageClass;

	UPROPERTY(EditAnywhere, Category = "Overlay Settings")
	TSubclassOf<class URoundStateOverlay> RoundStateClass;

	UPROPERTY(EditAnywhere, Category = "Overlay Settings")
	TSubclassOf<class URoundOverOverlay> RoundOverOverlayClass;

	UPROPERTY()
	class UPlayerOverlay* PlayerOverlay;

	UPROPERTY()
	class USpectatorOverlay* SpectatorOverlay;

	UPROPERTY()
	class UPauseMenu* PauseMenu;

	UPROPERTY()
	class UKillMessageOverlay* KillMessageOverlay;

	UPROPERTY()
	class URoundStateOverlay* RoundStateOverlay;

	UPROPERTY()
	class URoundOverOverlay* RoundOverOverlay;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	UTexture2D* Crosshair;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	UTexture2D* VTScanLine;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	UTexture2D* STTScanLine;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	UTexture2D* MarkPointer;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	FLinearColor TeamateColor;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	FLinearColor EnemyColor;

	UPROPERTY(EditAnywhere, Category = "HUD Settings")
	FLinearColor LockColor;

	class AFighter* OwnerFighter;

protected:
	virtual void BeginPlay() override;

	void CheckOwnerFighterStateAndPlayerOverlay();

	void DrawCrosshair();

	void DrawTargetMarkPointer();
	
	void GetMarkPointerDrawPos(FVector2D& DrawPos, float& RotDegree, float RelativeX, float RelativeY, FVector2D ScreenCenter, FVector2D TextureSize);

	void DrawRadarVTScanRange();

	void DrawRadarSTTScanRange();

public:
	void AddPlayerOverlay();
	void SetPlayerOverlayVisibility(bool bIsVisible);

	void AddSpectatorOverlay();
	void RemoveSpectatorOverlay();

	void AddPauseMenu();
	void RemovePauseMenu();

	void AddKillMessageOverlay(bool bIsKillMessage, const FString& PlayerName);
	void RemoveKillMessageOverlay();

	void AddRoundStateOverlay();
	void RemoveRoundStateOverlay();

	void AddRoundOverOverlay();
};
