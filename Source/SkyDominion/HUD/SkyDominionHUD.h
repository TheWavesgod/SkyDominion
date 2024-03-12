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

protected:
	virtual void BeginPlay() override;

	void AddPlayerOverlay();

	void DrawTargetMarkPointer();

	class AFighter* OwnerFighter;
	
};
