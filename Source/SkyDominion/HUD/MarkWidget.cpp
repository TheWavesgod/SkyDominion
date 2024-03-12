// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UMarkWidget::SetMarkState(ETargetMarkState MarkState)
{
	switch (MarkState)
	{
	case ETargetMarkState::TeamMate:
		SetDisplayColor(TeamateColor);
		Text_Ally->SetVisibility(ESlateVisibility::Visible);
		break;

	case ETargetMarkState::Enemy:
		SetDisplayColor(EnemyColor);
		Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case ETargetMarkState::Locked:
		SetDisplayColor(LockColor);
		Text_Ally->SetVisibility(ESlateVisibility::Collapsed);
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
