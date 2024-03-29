// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UMarkWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UMarkWidget::SetMarkState(ETargetMarkState MarkState)
{
	switch (MarkState)
	{
	case ETargetMarkState::Lost:
		SetVisibility(ESlateVisibility::Collapsed);
		break;

	case ETargetMarkState::TeamMate:
		SetVisibility(ESlateVisibility::Visible);
		SetDisplayColor(TeamateColor);
		Text_Ally->SetVisibility(ESlateVisibility::Visible);
		Text_Distance->SetVisibility(ESlateVisibility::Visible);
		break;

	case ETargetMarkState::Enemy:
		SetVisibility(ESlateVisibility::Visible);
		SetDisplayColor(EnemyColor);
		Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
		Text_Distance->SetVisibility(ESlateVisibility::Visible);
		break;

	case ETargetMarkState::Locked:
		SetDisplayColor(LockColor);
		Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case ETargetMarkState::RWSEnemy:
		SetRWSModeDisplay();
		break;
	}
}

void UMarkWidget::SetDisplayColor(FLinearColor color)
{
	Img_Mark->SetColorAndOpacity(color);
	Text_Distance->SetColorAndOpacity(color);
	Text_Ally->SetColorAndOpacity(color);
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

void UMarkWidget::SetRWSModeDisplay()
{
	SetVisibility(ESlateVisibility::Visible);
	SetDisplayColor(EnemyColor);
	Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
	Text_Distance->SetVisibility(ESlateVisibility::Collapsed);

	FTimerHandle MarkVisibleTimer;
	GetWorld()->GetTimerManager().SetTimer(MarkVisibleTimer, this, &ThisClass::SetRWSModeDisplayOff, 2.0f, false);
}

void UMarkWidget::SetRWSModeDisplayOff()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
