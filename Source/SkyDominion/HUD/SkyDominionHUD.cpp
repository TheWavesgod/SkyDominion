// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyDominionHUD.h"
#include "PlayerOverlay.h"
#include "SpectatorOverlay.h"
#include "PauseMenu.h"
#include "KillMessageOverlay.h"
#include "RoundStateOverlay.h"
#include "RoundOverOverlay.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "SkyDominion/Actor/RadarComponent.h"
#include "SkyDominion/Actor/AutoCannon.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "SkyDominion/Actor/SkyEnum.h"

void ASkyDominionHUD::BeginPlay()
{
	Super::BeginPlay();

	AddPlayerOverlay();
}

void ASkyDominionHUD::CheckOwnerFighterStateAndPlayerOverlay()
{
	if (!GetOwningPawn()) return;
	OwnerFighter = OwnerFighter == nullptr ? Cast<AFighter>(GetOwningPawn()) : OwnerFighter;

	PlayerOverlay->fighter = PlayerOverlay->fighter != OwnerFighter ? OwnerFighter : PlayerOverlay->fighter;
	OwnerFighter->SetPlayerOverlay(PlayerOverlay);
}

void ASkyDominionHUD::DrawHUD()
{
	Super::DrawHUD();

	CheckOwnerFighterStateAndPlayerOverlay();
	//DrawTargetMarkPointer();
	DrawCrosshair();
	DrawRadarVTScanRange();
	//DrawRadarSTTScanRange();
}

void ASkyDominionHUD::DrawCrosshair()
{
	if (!OwnerFighter) return;
	if (!Crosshair) return;

	FVector2D ScreenCenter = FVector2D(Canvas->SizeX / 2, Canvas->SizeY / 2);
	FVector2D TextureSize = FVector2D(Crosshair->GetSizeX(), Crosshair->GetSizeY()) * Canvas->SizeX / 2560 * 0.135f;

	AAutoCannon* OwnerCannon = OwnerFighter->GetAutoCannon();
	if (!OwnerCannon) return;
	FVector CrosshairWorldPos = OwnerCannon->GetActorLocation() + OwnerCannon->GetActorForwardVector() * 100000.0f;
	FVector2D CrosshairScreenPos;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayerController(), CrosshairWorldPos, CrosshairScreenPos))
	{
		FVector2D DrawPos = CrosshairScreenPos - FVector2D(TextureSize.X * 0.5f, TextureSize.Y * 0.5f);
		DrawPos *= FVector2D(1.0f, 1.007f);
		DrawTexture(Crosshair, DrawPos.X, DrawPos.Y, TextureSize.X, TextureSize.Y, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::Red, BLEND_Translucent);
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString("Draw CrossHair"));
	}
}

void ASkyDominionHUD::DrawTargetMarkPointer()
{
	if (OwnerFighter)
	{
		FVector2D ScreenCenter = FVector2D(Canvas->SizeX / 2, Canvas->SizeY / 2);
		FVector2D TextureSize = FVector2D(MarkPointer->GetSizeX(), MarkPointer->GetSizeY()) * 1.2f * Canvas->SizeX / 2560;

		TArray<AActor*> TargetList;
		for (const AActor* target : TargetList)
		{
			if (!target) { continue; }
			if (target->ActorHasTag("FighterJet"))
			{
				FVector TargetLocation = target->GetActorLocation();	
				FVector2D TargetScreenPos;
				bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayerController(), TargetLocation, TargetScreenPos);

				if (!bIsOnScreen || TargetScreenPos.X < 0 || TargetScreenPos.Y < 0 || TargetScreenPos.X > Canvas->SizeX || TargetScreenPos.Y > Canvas->SizeY)
				{
					// Calculate Pos And Rot
					FVector2D DrawPos;
					float RotDegree;
					if (!bIsOnScreen/*TargetScreenPos.X == 0 && TargetScreenPos.Y == 0*/)
					{
						FVector CameraLoc = GetOwningPlayerController()->PlayerCameraManager->GetCameraLocation();
						FVector TargetDirection = (TargetLocation - CameraLoc).GetSafeNormal();
						FVector TargetRelativeDirection = GetOwningPlayerController()->PlayerCameraManager->GetCameraRotation().UnrotateVector(TargetDirection);
						FVector2D VirtualScreenPos = FVector2D(TargetRelativeDirection.Y, TargetRelativeDirection.Z) * Canvas->SizeX;
						if (VirtualScreenPos.X == 0 && VirtualScreenPos.Y == 0)
						{
							VirtualScreenPos = FVector2D(0.0f, -Canvas->SizeY);
						}
						GetMarkPointerDrawPos(DrawPos, RotDegree, VirtualScreenPos.X, VirtualScreenPos.Y, ScreenCenter, TextureSize);
					}
					else
					{
						FVector2D TargetCenterRelativePos = TargetScreenPos - ScreenCenter;
						GetMarkPointerDrawPos(DrawPos, RotDegree, TargetCenterRelativePos.X, TargetCenterRelativePos.Y, ScreenCenter, TextureSize);
					}

					// Get Colot
					FLinearColor DrawColor;
					if (target->ActorHasTag("TeamMate"))
					{
						DrawColor = TeamateColor;
					}
					if (target->ActorHasTag("Enemy"))
					{
						DrawColor = EnemyColor;
					}

					//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Draw Pos: %f, %f"), DrawPos.X, DrawPos.Y));
					DrawTexture(MarkPointer, DrawPos.X, DrawPos.Y, TextureSize.X, TextureSize.Y, 0.0f, 0.0f, 1.0f, 1.0f, DrawColor, BLEND_Translucent, 1.2f, false, RotDegree, FVector2D(0.5, 0.5));
				}
			}
		}
	}
}

void ASkyDominionHUD::GetMarkPointerDrawPos(FVector2D& DrawPos, float& RotDegree, float RelativeX, float RelativeY, FVector2D ScreenCenter, FVector2D TextureSize)
{
	RotDegree = FMath::RadiansToDegrees(FMath::Atan2(RelativeY, RelativeX)) + 90.0f;

	float PosProportion = FMath::Abs(RelativeX) / Canvas->SizeX > FMath::Abs(RelativeY) / Canvas->SizeY ?
		ScreenCenter.X / FMath::Abs(RelativeX) :
		ScreenCenter.Y / FMath::Abs(RelativeY);

	FVector2D NewEdgePos = FVector2D(RelativeX, RelativeY) * PosProportion;

	float RotPosOffset = TextureSize.Y * 0.5f;
	FVector2D DeltaV2D = NewEdgePos.GetSafeNormal() * RotPosOffset;

	FVector2D DrawCenter = NewEdgePos - DeltaV2D;

	FVector2D TextureDrawOffset = TextureSize * 0.5f;
	DrawPos = DrawCenter - TextureDrawOffset + ScreenCenter;
}

void ASkyDominionHUD::DrawRadarVTScanRange()
{
	if (!OwnerFighter) return;
	if (OwnerFighter->GetRadarComponent()->GetRadarModeEnum() != ERadarMode::VT) return;
	if (!VTScanLine) return;

	FVector2D TextureSize = FVector2D(VTScanLine->GetSizeX(), VTScanLine->GetSizeY()) * Canvas->SizeX / 1920;

	float MaxDis = OwnerFighter->GetRadarComponent()->MaximumRadarSearchRadius;
	float ScanAngle = OwnerFighter->GetRadarComponent()->VTModeScanAngle;

	float YRatio = FMath::Tan(FMath::DegreesToRadians(ScanAngle * 0.5f));

	FVector RangeDirectionRight = OwnerFighter->GetTransform().TransformVector(FVector(1.0f, YRatio, 0.0f).GetSafeNormal());
	FVector RangeDirectionLeft = OwnerFighter->GetTransform().TransformVector(FVector(1.0f, -YRatio, 0.0f).GetSafeNormal());

	FVector RangePosRight = OwnerFighter->GetActorLocation() + RangeDirectionRight * MaxDis * 100.0f;
	FVector RangePosLeft = OwnerFighter->GetActorLocation() + RangeDirectionLeft * MaxDis * 100.0f;

	FVector2D  RangeScreenPosRight;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayerController(), RangePosRight, RangeScreenPosRight))
	{
		FVector2D DrawPos = RangeScreenPosRight - TextureSize * 0.5f;
		DrawTexture(VTScanLine, DrawPos.X, DrawPos.Y, TextureSize.X, TextureSize.Y, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::Green, BLEND_Translucent);
	}

	FVector2D  RangeScreenPosLeft;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayerController(), RangePosLeft, RangeScreenPosLeft))
	{
		FVector2D DrawPos = RangeScreenPosLeft - TextureSize * 0.5f;
		DrawTexture(VTScanLine, DrawPos.X, DrawPos.Y, TextureSize.X, TextureSize.Y, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::Green, BLEND_Translucent);
	}
}

void ASkyDominionHUD::DrawRadarSTTScanRange()
{
	if (!OwnerFighter) return;
	if (OwnerFighter->GetRadarComponent()->GetRadarModeEnum() != ERadarMode::STT) return;
	if (!STTScanLine) return;
}

void ASkyDominionHUD::AddPlayerOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && PlayerOverlayClass)
	{
		PlayerOverlay = PlayerOverlay == nullptr ? CreateWidget<UPlayerOverlay>(PlayerController, PlayerOverlayClass) : PlayerOverlay;
		PlayerOverlay->AddToViewport();

		APawn* controledPawn = PlayerController->GetPawn();
		AFighter* fighter = Cast<AFighter>(controledPawn);
		if (fighter)
		{
			PlayerOverlay->fighter = fighter;
			OwnerFighter = fighter;
			fighter->SetPlayerOverlay(PlayerOverlay);
		}
	}
}

void ASkyDominionHUD::SetPlayerOverlayVisibility(bool bIsVisible)
{
	if (!PlayerOverlay) return;

	if (bIsVisible)
	{
		PlayerOverlay->AddToViewport();
	}
	else
	{
		PlayerOverlay->RemoveFromViewport();
	}
}

void ASkyDominionHUD::AddSpectatorOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && SpectatorOverlayClass)
	{
		SpectatorOverlay = SpectatorOverlay == nullptr ? CreateWidget<USpectatorOverlay>(PlayerController, SpectatorOverlayClass) : SpectatorOverlay;
		SpectatorOverlay->AddToViewport();
		SpectatorOverlay->Init();
	}
}

void ASkyDominionHUD::RemoveSpectatorOverlay()
{
	if (SpectatorOverlay)
	{
		SpectatorOverlay->RemoveFromViewport();
	}
}

void ASkyDominionHUD::AddPauseMenu()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && PauseMenuClass)
	{
		PauseMenu = PauseMenu == nullptr ? CreateWidget<UPauseMenu>(PlayerController, PauseMenuClass) : PauseMenu;
		PauseMenu->AddToViewport();
	}
}

void ASkyDominionHUD::RemovePauseMenu()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromViewport();
	}
}

void ASkyDominionHUD::AddKillMessageOverlay(bool bIsKillMessage, const FString& PlayerName)
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && KillMessageClass)
	{
		KillMessageOverlay = KillMessageOverlay == nullptr ? CreateWidget<UKillMessageOverlay>(PlayerController, KillMessageClass) : KillMessageOverlay;
		KillMessageOverlay->Init(bIsKillMessage, PlayerName);
		KillMessageOverlay->AddToViewport();

		FTimerHandle RemoveHandle;
		GetWorld()->GetTimerManager().SetTimer(RemoveHandle, this, &ThisClass::RemoveKillMessageOverlay, 4.5f, false);
	}
}

void ASkyDominionHUD::RemoveKillMessageOverlay()
{
	if (KillMessageOverlay)
	{
		KillMessageOverlay->RemoveFromParent();
	}
}

void ASkyDominionHUD::AddRoundStateOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && RoundStateClass)
	{
		RoundStateOverlay = RoundStateOverlay == nullptr ? CreateWidget<URoundStateOverlay>(PlayerController, RoundStateClass) : RoundStateOverlay; 
		RoundStateOverlay->bCanUpdate = true;
		RoundStateOverlay->UpdatePlayerList();
		RoundStateOverlay->AddToViewport(2);
	}
}

void ASkyDominionHUD::RemoveRoundStateOverlay()
{
	if (RoundStateOverlay)
	{
		RoundStateOverlay->bCanUpdate = false;
		RoundStateOverlay->RemoveFromParent();
	}
}

void ASkyDominionHUD::AddRoundOverOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && RoundOverOverlayClass)
	{
		RoundOverOverlay = RoundOverOverlay == nullptr ? CreateWidget<URoundOverOverlay>(PlayerController, RoundOverOverlayClass) : RoundOverOverlay;
		RoundOverOverlay->InitRoundOverOverlay();
		RoundOverOverlay->AddToViewport(2);
	}
}
