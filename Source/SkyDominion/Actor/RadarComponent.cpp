

#include "RadarComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "SkyDominion/Actor/Missile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "SkyDominion/HUD/MarkWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

URadarComponent::URadarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	OwnerFighter = GetOwner<AFighter>();
	if (OwnerFighter)
	{
		DetectCollision = OwnerFighter->GetRadarDetectCollision();
	}

	for (int i = 0; i < 2; ++i)
	{
		EnemyFightersDetected.Emplace(nullptr);
	}
}

void URadarComponent::BeginPlay()
{
	Super::BeginPlay();

	DetectCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectCollision->SetCollisionObjectType(ECC_Visibility);	
	DetectCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectCollision->IgnoreActorWhenMoving(OwnerFighter, true);

	InitiateRadar();
}

void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InitiateRadar();
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString("Current Mode ") + GetCurrentRadarMode());
	/*if (OwnerFighter->IsLocallyControlled())
	{
		TSet<AActor*> otherActors;
		DetectCollision->GetOverlappingActors(otherActors);
		if (OwnerFighter->HasAuthority())
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Sever: Detect Actor Num: %d"), otherActors.Num()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Client: Detect Actor Num: %d"), otherActors.Num()));
		}
	}*/

	CheckLockState(DeltaTime);
}

void URadarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(URadarComponent, DetectedTargets, COND_OwnerOnly);
}

void URadarComponent::InitiateRadar()
{
	if (bRadarBeInitiated) return;

	if (!OwnerFighter->IsLocallyControlled()) return;

	bRadarBeInitiated = true;
	DetectCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	DetectCollision->SetGenerateOverlapEvents(true);
	DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectCollsionBeginOverlap);
	DetectCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectCollsionEndOverlap);
	DetectCollision->SetSphereRadius(MaximumRadarSearchRadius * 100.0f);
	DetectCollision->bHiddenInGame = true;

	FTimerHandle RadarInfoUpdate;
	GetWorld()->GetTimerManager().SetTimer(RadarInfoUpdate, this, &ThisClass::CheckCollisionList, CheckListFrequency, true);
}

void URadarComponent::OnDetectCollsionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ServerUpdateTargetsList(OtherActor, true);
}

void URadarComponent::OnDetectCollsionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	LocalNewTargetLost(OtherActor);
}


void URadarComponent::LocalNewTargetDetected(AActor* Target)
{
	//UE_LOG(LogTemp, Warning, TEXT("Target detected"));
	
	// Check if is Fighter
	if (Target->ActorHasTag(FName("Fighter")))
	{
		AFighter* FighterTarget = static_cast<AFighter*>(Target);
		if (FighterTarget)
		{
			if (OwnerFighter->bInRedTeam == FighterTarget->bInRedTeam)
			{
				SetFighterMarkState(FighterTarget, ETargetMarkState::TeamMate, FMath::Abs((FighterTarget->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f));
			}
			else // Not Teammate
			{
				// Define how to show enemy on screen depending on which Radar mode is
				switch (CurrentRadarMode)
				{
				case ERadarMode::RWS:
					DetectFighterOnRWSMode(FighterTarget);
					break;

				case ERadarMode::VT:
					DetectFighterOnVTMode(FighterTarget);
					break;

				case ERadarMode::STT:
					DetectFighterOnSTTMode(FighterTarget);
					break;
				}
			}
		}
	}
	else if (Target->ActorHasTag(FName("Missile")))
	{
		AMissile* MissileTarget = static_cast<AMissile*>(Target);
		if (MissileTarget)
		{
			if (MissileTarget->GetMissileHasFired())
			{
				if (MissileTarget->bInRedTeam == OwnerFighter->bInRedTeam)
				{
					MissileTarget->SetMissileMarkVisbility(true, true);
				}
				else
				{
					if ((MissileTarget->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() * 0.01f < 15000.0f)
					{
						MissileTarget->SetMissileMarkVisbility(true, false);
					}
				}
			}
		}
	}
	/*else if (Target->ActorHasTag(FName("Flare")))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("Detect Flare!!"));
	}*/
}

void URadarComponent::LocalNewTargetLost(AActor* Target)
{
	// Check if is Fighter
	if (Target->ActorHasTag(FName("Fighter")))
	{
		AFighter* FighterTarget = static_cast<AFighter*>(Target);
		if (FighterTarget)
		{
			SetFighterMarkState(FighterTarget, ETargetMarkState::Lost);
		}
	}
	
	if (Target->ActorHasTag(FName("Missile")))
	{
		AMissile* MissileTarget = static_cast<AMissile*>(Target);
		if (MissileTarget)
		{
			MissileTarget->SetMissileMarkVisbility(false, true);
		}
	}
}

void URadarComponent::SetFighterMarkState(AFighter* target, ETargetMarkState MarkState, float distance)
{
	UMarkWidget* MarkWidget = StaticCast<UMarkWidget*>(target->GetMarkWidget()->GetWidget());
	if (MarkWidget) {
		MarkWidget->SetMarkState(MarkState);
		MarkWidget->Distance = distance;
		MarkWidget->Name = target->FighterDisplayName.ToString();
	}

	if (MarkState == ETargetMarkState::Enemy) AddDetectedEnemy(target);
	if (MarkState == ETargetMarkState::Lost) DetectedEnemyLost(target);
}


void URadarComponent::CheckCollisionList()
{
	TSet<AActor*> otherActors;
	DetectCollision->GetOverlappingActors(otherActors);

	if (CurrentRadarMode == ERadarMode::STT)
	{
		if (otherActors.Num() == 0)
		{
			OwnerFighter->ShutDownRadarLockSound();
			CurrentRadarMode = ERadarMode::VT;
			TargetBeingLocked = nullptr;
			SetServerLockedTarget(nullptr);
			return;
		}

		if (!IsValid(TargetBeingLocked))
		{
			OwnerFighter->ShutDownRadarLockSound();
			CurrentRadarMode = ERadarMode::VT;
			TargetBeingLocked = nullptr;
			SetServerLockedTarget(nullptr);
		}
	}

	for (auto i : otherActors)
	{
		if (!i) continue;
		
		if (CheckCollisionBetweenTargetAndSelf(i))
			LocalNewTargetLost(i);
		else
			LocalNewTargetDetected(i);
	}
}

bool URadarComponent::CheckCollisionBetweenTargetAndSelf(const AActor* target)
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerFighter);
	QueryParams.AddIgnoredActor(target);
	GetWorld()->LineTraceSingleByChannel(HitResult, DetectCollision->GetComponentLocation(), target->GetActorLocation() + target->GetActorUpVector() * 300.0f, ECC_WorldStatic, QueryParams);

	/*if(HitResult.bBlockingHit)
		UE_LOG(LogTemp, Warning, TEXT("Target block"));*/

	return HitResult.bBlockingHit;
}

void URadarComponent::DetectFighterOnRWSMode(AFighter* target)
{
	if (target->RWSDetectTimer <= 0.0f)
	{
		ActiveTargetRWSScanedAlert(target);
		target->RWSDetectTimer = RWSScaningPeriod;
		target->RWSDisapearTimer = 2.0f;
		SetFighterMarkState(target, ETargetMarkState::RWSEnemy);
		FTimerHandle MarkVisibleTimer;
	}

	if (target->RWSDisapearTimer <= 0.0f)
	{
		SetFighterMarkState(target, ETargetMarkState::Lost);
	}
}

void URadarComponent::DetectFighterOnVTMode(AFighter* target)
{
	FVector TargetPos = target->GetActorLocation();
	FVector TargetRelativePos = OwnerFighter->GetTransform().InverseTransformPosition(TargetPos);

	bool InVTScanRange = false;
	if (TargetRelativePos.X > 0)
	{
		float HAngle = FMath::RadiansToDegrees(FMath::Atan(FMath::Abs(TargetRelativePos.Y) / FMath::Abs(TargetRelativePos.X)));
		//UE_LOG(LogTemp, Warning, TEXT("HAngle: %f"), HAngle);
		if (HAngle < VTModeScanAngle * 0.5f)
		{
			InVTScanRange = true;

			/*float VAngle = FMath::Abs(FMath::RadiansToDegrees(FMath::Atan2(TargetRelativePos.X, TargetRelativePos.Z)));
			if (VAngle < VTModeScanAngle * 0.5f)
			{
				InVTScanRange = true;
			}*/
		}
	}

	if (InVTScanRange)
	{
		SetFighterMarkState(target, ETargetMarkState::Enemy, FMath::Abs((target->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f));
		ActiveTargetVTScanedAlert(target);
	}
	else
	{
		SetFighterMarkState(target, ETargetMarkState::Lost);
	}
}

void URadarComponent::DetectFighterOnSTTMode(AFighter* target)
{
	if (!IsValid(TargetBeingLocked))
	{
		OwnerFighter->ShutDownRadarLockSound();
		CurrentRadarMode = ERadarMode::VT;
		TargetBeingLocked = nullptr;
		return;
	}

	if (target != TargetBeingLocked)
	{
		SetFighterMarkState(target, ETargetMarkState::Lost);
		return;
	}

	FVector TargetPos = target->GetActorLocation();
	FVector TargetRelativePos = OwnerFighter->GetTransform().InverseTransformPosition(TargetPos);

	bool InSTTScanRange = false;
	if (TargetRelativePos.X > 0)
	{
		float Ytan = FVector(0.0f, TargetRelativePos.Y, TargetRelativePos.Z).Size();
		float CAngle = FMath::RadiansToDegrees(FMath::Atan(FMath::Abs(Ytan) / FMath::Abs(TargetRelativePos.X)));
		//UE_LOG(LogTemp, Warning, TEXT("CAngle: %f"), CAngle);
		if (CAngle < STTModeScanAngle * 0.5f)
		{
			InSTTScanRange = true;
		}
	}

	if (!CheckTargetInRange(EnemyFightersDetected[LockTargetIndex], OwnerFighter->GetCurrentMissileRange() * 1000.0f))
	{
		InSTTScanRange = false;
	}

		
	if (InSTTScanRange)
	{
		SetFighterMarkState(target, ETargetMarkState::Locked, FMath::Abs((target->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f));
		ActiveTargetSTTLockedAlert(TargetBeingLocked);
		OwnerFighter->ShowTargetLocked();
	}
	else
	{
		SetFighterMarkState(target, ETargetMarkState::Lost);
		SetServerLockedTarget(nullptr);
		OwnerFighter->ShutDownRadarLockSound();
		OwnerFighter->ActiveTargetLostAlertSound();
		OwnerFighter->ShowTargetLost();
		CurrentRadarMode = ERadarMode::VT;
		TargetBeingLocked = nullptr;
	}
}


void URadarComponent::AddDetectedEnemy(AFighter* Target)
{
	if (EnemyFightersDetected.Find(Target) != INDEX_NONE) return;
	 
	for (auto &i : EnemyFightersDetected)
	{
		if (i == nullptr)
		{
			i = Target;
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Add New Target in %d "), i) + i->GetName());
			break;
		}
	}
}

void URadarComponent::DetectedEnemyLost(AFighter* Target)
{
	int32 index = EnemyFightersDetected.Find(Target);

	if (index == INDEX_NONE) return;
	
	EnemyFightersDetected[index] = nullptr;
}

bool URadarComponent::CheckTargetInRange(AActor* Target, float DetectRange)
{
	if (!OwnerFighter || !Target) return false;

	float distance = FMath::Abs((Target->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f);

	return distance < DetectRange;
}

FString URadarComponent::GetCurrentRadarMode() const
{
	FString CurrentMode;
	switch (CurrentRadarMode)
	{
	case ERadarMode::RWS:
		CurrentMode = FString("RWS");
		break;

	case ERadarMode::VT:
		CurrentMode = FString("VT");
		break;

	case ERadarMode::STT:
		CurrentMode = FString("STT");
		break;
	}
	return CurrentMode;
}

void URadarComponent::ChangeRadarMode()
{
	if (CurrentRadarMode == ERadarMode::STT) return;

	CurrentRadarMode = CurrentRadarMode == ERadarMode::RWS ?
		ERadarMode::VT : ERadarMode::RWS;
}

void URadarComponent::StartLockTarget()
{
	if (CurrentRadarMode == ERadarMode::STT)
	{
		OwnerFighter->ShutDownRadarLockSound();
		CurrentRadarMode = ERadarMode::VT;
		TargetBeingLocked = nullptr;
		SetServerLockedTarget(nullptr);
		return;
	}

	if (EnemyFightersDetected[0] != nullptr && EnemyFightersDetected[1] != nullptr)
	{
		SetFighterMarkState(EnemyFightersDetected[LockTargetIndex], ETargetMarkState::Lost);
	}

	LockTargetIndex = LockTargetIndex == 0 ? 1 : 0;
	
	if (EnemyFightersDetected[LockTargetIndex] == nullptr)
	{
		LockTargetIndex = LockTargetIndex == 0 ? 1 : 0;
		if (EnemyFightersDetected[LockTargetIndex] == nullptr)
		{
			// Send NO Target Signal
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("No target!"));
			OwnerFighter->ActiveUnvalidMoveAlertSound();
			OwnerFighter->ShowNoTargetAlert();
			return;
		}
	}

	if (!CheckTargetInRange(EnemyFightersDetected[LockTargetIndex], OwnerFighter->GetCurrentMissileRange() * 1000.0f))
	{
		// Send Missile Out of Range Sound
		OwnerFighter->ActiveUnvalidMoveAlertSound();
		OwnerFighter->ShowMissileOutofRange();
		return;
	}

	LockingTimeHandle = TimeToLocked;
	OwnerFighter->ActiveTargetLockingSound();
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("Start Locking!") + EnemyFightersDetected[LockTargetIndex]->GetName());
}

void URadarComponent::CheckLockState(float DeltaTime)
{
	if (LockingTimeHandle == 0.0f) return;

	LockingTimeHandle = FMath::Clamp(LockingTimeHandle - DeltaTime, 0.0f, TimeToLocked);

	if (EnemyFightersDetected[LockTargetIndex] == nullptr)
	{
		// Target lost
		OwnerFighter->ShutDownRadarLockSound();
		OwnerFighter->ActiveTargetLostAlertSound();
		OwnerFighter->ShowTargetLost();
		LockingTimeHandle = 0.0f;
	}
	else
	{
		if (LockingTimeHandle == 0.0f)
		{
			SetFighterMarkState(EnemyFightersDetected[LockTargetIndex], ETargetMarkState::Locked, FMath::Abs((EnemyFightersDetected[LockTargetIndex]->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f));
			TargetBeingLocked = EnemyFightersDetected[LockTargetIndex];
			SetServerLockedTarget(EnemyFightersDetected[LockTargetIndex]);
			CurrentRadarMode = ERadarMode::STT;
			OwnerFighter->ActiveTargetLockedSound();
		}
		else
		{
			if (!CheckTargetInRange(EnemyFightersDetected[LockTargetIndex], OwnerFighter->GetCurrentMissileRange() * 1000.0f))
			{
				// Send Missile Out of Range Sound
				OwnerFighter->ShutDownRadarLockSound();
				OwnerFighter->ActiveTargetLostAlertSound();
				OwnerFighter->ShowTargetLost();
				LockingTimeHandle = 0.0f;
				SetFighterMarkState(EnemyFightersDetected[LockTargetIndex], ETargetMarkState::Lost);
				return;
			}
			SetFighterMarkState(EnemyFightersDetected[LockTargetIndex], ETargetMarkState::Locking, FMath::Abs((EnemyFightersDetected[LockTargetIndex]->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f));
		}
	}
}

AFighter* URadarComponent::GetLockedTarget() const
{
	if (IsValid(TargetBeingLocked))
	{
		return TargetBeingLocked;
	}
	else
	{
		return nullptr;
	}
}



void URadarComponent::ActiveTargetRWSScanedAlert_Implementation(AFighter* target)
{
	target->ActivateRwsBeScanedAlert();
}

void URadarComponent::ActiveTargetVTScanedAlert_Implementation(AFighter* target)
{
	target->ActivateVTBeScanedAlert();
}

void URadarComponent::ActiveTargetSTTLockedAlert_Implementation(AFighter* target)
{
	target->ActivateSTTBeLockedAlert();
}

void URadarComponent::DeactiveTargetSTTLockedAlert_Implementation(AFighter* target)
{
	target->DeactivateSTTBeLockedAlert();
}

void URadarComponent::SetServerLockedTarget_Implementation(AFighter* target)
{
	TargetBeingLocked = target;
}




