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

	UPROPERTY(EditAnywhere, Category = "Player Overlay")
	TSubclassOf<class UUserWidget> PlayerOverlayClass;

	class UPlayerOverlay* PlayerOverlay;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	UTexture2D* MarkPointer;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	FLinearColor TeamateColor;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	FLinearColor EnemyColor;

	UPROPERTY(EditAnywhere, Category = "Mark Pointer")
	FLinearColor LockColor;

protected:
	virtual void BeginPlay() override;

	void AddPlayerOverlay();

	void DrawTargetMarkPointer();
	
	void GetMarkPointerDrawPos(FVector2D& DrawPos, float& RotDegree, float RelativeX, float RelativeY, FVector2D ScreenCenter, FVector2D TextureSize);

	class AFighter* OwnerFighter;
	
};
