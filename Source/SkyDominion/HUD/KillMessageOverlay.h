// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillMessageOverlay.generated.h"

class UHorizontalBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SKYDOMINION_API UKillMessageOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(bool bIsKillMessage, const FString& PlayerName);

protected:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Box_KillMessage;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Box_DefeatMessage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_VictimName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_AttackName;
private:


};
