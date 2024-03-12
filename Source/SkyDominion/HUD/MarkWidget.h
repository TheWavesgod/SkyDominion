// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MarkWidget.generated.h"

class UTextBlock;
class UImage;

UENUM()
enum class ETargetMarkState : uint8
{
	TeamMate UMETA(DisplayName = "Team Mate"),
	Enemy UMETA(DisplayName = "Enemy"),
	Locked  UMETA(DisplayName = "Enemy"),

	TargetMarkStateMax UMETA(DisplayName = "TargetMarkStateMax")
};

/**
 * 
 */
UCLASS()
class SKYDOMINION_API UMarkWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMarkState(ETargetMarkState MarkState);

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	FLinearColor TeamateColor;

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	FLinearColor EnemyColor;

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	FLinearColor LockColor;

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	float Distance = 0.0f;

protected:
	//virtual bool Initialize() override;
	void SetDisplayColor(FLinearColor color);

	UFUNCTION(BlueprintCallable)
	void UpdateDistanceText();

private:
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Mark;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Distance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Ally;
	
};
