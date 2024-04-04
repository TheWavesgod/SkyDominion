// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundStateOverlay.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class SKYDOMINION_API URoundStateOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	bool bCanUpdate = false;

	void UpdatePlayerList();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class ASkyGameState* SkyGameState;

private:
	void WritePlayerState(class ASkyPlayerState* targetState, int32 index);

	void SetTargetText(UTextBlock* target, const FString& Val);

	// Red1
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name_Red1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Kill_Red1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Dead_Red1;

	// Red2
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name_Red2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Kill_Red2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Dead_Red2;

	// Blue1
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name_Blue1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Kill_Blue1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Dead_Blue1;

	// Blue2
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name_Blue2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Kill_Blue2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Dead_Blue2;
};
