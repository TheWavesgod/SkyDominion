// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UMarkWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);


}

void UMarkWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bImgMarkCanFlick)
	{
		MarkImgFlickHandle = FMath::Clamp(MarkImgFlickHandle - InDeltaTime, 0.0f, LockingFlickGap);

		if (MarkImgFlickHandle == 0.0f)
		{
			MarkImgFlickHandle = LockingFlickGap;
			ToggleImgMarkVisibility();
		}
	}
}

void UMarkWidget::SetMarkState(ETargetMarkState MarkState)
{
	//if (MarkState == CurrentState) return;

	if (CurrentState == ETargetMarkState::Locking && MarkState == ETargetMarkState::Enemy) return;

	if (MarkState != ETargetMarkState::Locking) bImgMarkCanFlick = false;

	CurrentState = MarkState;

	switch (MarkState)
	{
	case ETargetMarkState::Lost:
		SetVisibility(ESlateVisibility::Collapsed);
		break;

	case ETargetMarkState::TeamMate:
		SetVisibility(ESlateVisibility::Visible);
		SetDisplayColor(TeamateColor);
		Img_Mark->SetVisibility(ESlateVisibility::Visible);
		Text_Ally->SetVisibility(ESlateVisibility::Visible);
		Text_Distance->SetVisibility(ESlateVisibility::Visible);
		Text_Name->SetVisibility(ESlateVisibility::Visible);
		break;

	case ETargetMarkState::RWSEnemy:
		SetRWSModeDisplay();
		break;

	case ETargetMarkState::Enemy:
		SetVisibility(ESlateVisibility::Visible);
		SetDisplayColor(EnemyColor);
		Img_Mark->SetVisibility(ESlateVisibility::Visible);
		Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
		Text_Distance->SetVisibility(ESlateVisibility::Visible);
		Text_Name->SetVisibility(ESlateVisibility::Visible);
		break;

	case ETargetMarkState::Locking:
		SetLockingModeDisplay();
		break;

	case ETargetMarkState::Locked:
		SetVisibility(ESlateVisibility::Visible);
		SetDisplayColor(LockColor);
		Img_Mark->SetVisibility(ESlateVisibility::Visible);
		Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
		Text_Distance->SetVisibility(ESlateVisibility::Visible);
		Text_Name->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}

void UMarkWidget::SetDisplayColor(FLinearColor color)
{
	Img_Mark->SetColorAndOpacity(color);
	Text_Distance->SetColorAndOpacity(color);
	Text_Ally->SetColorAndOpacity(color);
	Text_Name->SetColorAndOpacity(color);
}

void UMarkWidget::UpdateDistanceText()
{
	FString NewDistanceString = FString::FromInt(FMath::CeilToInt(Distance)) + FString("m");
	FText NewDistanceText = FText::FromString(NewDistanceString);
	if (!Text_Distance->GetText().EqualTo(NewDistanceText))
	{
		Text_Distance->SetText(NewDistanceText);
	}
}

void UMarkWidget::UpdateDisplayName()
{
	FText displayName = FText::FromString(Name);
	if (!Text_Name->GetText().EqualTo(displayName))
	{
		Text_Name->SetText(displayName);
	}
}

void UMarkWidget::SetRWSModeDisplay()
{
	SetVisibility(ESlateVisibility::Visible);
	SetDisplayColor(EnemyColor);
	Img_Mark->SetVisibility(ESlateVisibility::Visible);
	Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
	Text_Distance->SetVisibility(ESlateVisibility::Collapsed);
	Text_Name->SetVisibility(ESlateVisibility::Collapsed);
}

void UMarkWidget::SetLockingModeDisplay()
{
	SetDisplayColor(LockColor);
	//Img_Mark->SetVisibility(ESlateVisibility::Visible);
	Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
	Text_Distance->SetVisibility(ESlateVisibility::Visible);
	Text_Name->SetVisibility(ESlateVisibility::Visible);
	bImgMarkCanFlick = true;
}

void UMarkWidget::ToggleImgMarkVisibility()
{
	if (Img_Mark->GetVisibility() == ESlateVisibility::Visible)
	{
		Img_Mark->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Img_Mark->SetVisibility(ESlateVisibility::Visible);
	}
}
