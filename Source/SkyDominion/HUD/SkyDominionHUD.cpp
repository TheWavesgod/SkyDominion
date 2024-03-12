// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyDominionHUD.h"
#include "PlayerOverlay.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "SkyDominion/Actor/RadarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"

void ASkyDominionHUD::BeginPlay()
{
	Super::BeginPlay();

	AddPlayerOverlay();
}

void ASkyDominionHUD::AddPlayerOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && PlayerOverlayClass)
	{
		PlayerOverlay = CreateWidget<UPlayerOverlay>(PlayerController, PlayerOverlayClass);
		PlayerOverlay->AddToViewport();

		APawn* controledPawn = PlayerController->GetPawn();
		AFighter* fighter = Cast<AFighter>(controledPawn);
		if (fighter)
		{
			PlayerOverlay->fighter = fighter;
			OwnerFighter = fighter;
		}
	}
}

void ASkyDominionHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		DrawTargetMarkPointer();
	}
}

void ASkyDominionHUD::DrawTargetMarkPointer()
{
	if (OwnerFighter)
	{
		FVector2D ScreenCenter = FVector2D(Canvas->SizeX / 2, Canvas->SizeY / 2);
		DrawTexture(MarkPointer, ScreenCenter.X, ScreenCenter.Y, MarkPointer->GetSizeX(), MarkPointer->GetSizeY(), 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::Blue, BLEND_Translucent, 1.3f, false, 0.0f, FVector2D(0.5, 0.5));

		for (const AActor* target : OwnerFighter->GetRadarComponent()->DetectedTargetsInMemory)
		{
			if (target->ActorHasTag("FighterJet"))
			{
				FVector TargetLocation = target->GetActorLocation();	
				FVector2D TargetScreenPos;
				bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayerController(), TargetLocation, TargetScreenPos);

				if (!bIsOnScreen || TargetScreenPos.X < 0 || TargetScreenPos.Y < 0 || TargetScreenPos.X > Canvas->SizeX || TargetScreenPos.Y > Canvas->SizeY)
				{
					if (TargetScreenPos.X == 0 && TargetScreenPos.Y == 0)
					{

					}
					else
					{
						FVector2D TargetCenterRelativePos = TargetScreenPos - ScreenCenter;
						float RotDegree = FMath::RadiansToDegrees(FMath::Atan2(TargetCenterRelativePos.Y, TargetCenterRelativePos.X));
						float PosProportion = FMath::Abs(TargetCenterRelativePos.X) > ScreenCenter.X ?
							ScreenCenter.X / FMath::Abs(TargetCenterRelativePos.X) :
							ScreenCenter.Y / FMath::Abs(TargetCenterRelativePos.Y);
						FVector2D TargetScreenEdgePos = TargetCenterRelativePos * PosProportion + ScreenCenter;
					}
				}
				//DrawTexture()
				//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Target Screen Loc: %d, %d"), FMath::RoundToInt(TargetScreenPos.X), FMath::RoundToInt(TargetScreenPos.Y)));
			}
		}
	}
}


