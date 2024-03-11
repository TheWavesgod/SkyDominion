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
		break;

	case ETargetMarkState::Enemy:
		SetDisplayColor(EnemyColor);
		break;

	case ETargetMarkState::Locked:
		SetDisplayColor(LockColor);
		break;
	}
}

void UMarkWidget::SetDisplayColor(FLinearColor color)
{
	Img_Mark->SetColorAndOpacity(color);
	Text_Distance->SetColorAndOpacity(color);
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
