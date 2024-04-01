// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkyDominion/Actor/SkyEnum.h"

#include "MarkWidget.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class SKYDOMINION_API UMarkWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMarkState(ETargetMarkState MarkState);
	void SetMissileMarkState(bool bInSameTeam);

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	float LockingFlickGap = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	FLinearColor TeamateColor;

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	FLinearColor EnemyColor;

	UPROPERTY(EditAnywhere, Category = "Mark Config")
	FLinearColor LockColor;

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	float Distance = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	FString Name;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//virtual bool Initialize() override;
	void SetDisplayColor(FLinearColor color);

	UFUNCTION(BlueprintCallable)
	void UpdateDistanceText();

	UFUNCTION(BlueprintCallable)
	void UpdateDisplayName();

	void SetRWSModeDisplay();

	void SetLockingModeDisplay();
	void ToggleImgMarkVisibility();
	bool bImgMarkCanFlick = false;
	float MarkImgFlickHandle = 0.0f;

	ETargetMarkState CurrentState = ETargetMarkState::Lost;

private:
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Mark;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Distance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Ally;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name;
};
