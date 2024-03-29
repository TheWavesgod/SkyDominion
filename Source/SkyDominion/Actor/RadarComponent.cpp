

#include "RadarComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "SkyDominion/HUD/MarkWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

URadarComponent::URadarComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	OwnerFighter = GetOwner<AFighter>();
	if (OwnerFighter)
	{
		DetectCollision = OwnerFighter->GetRadarDetectCollision();
	}
}

void URadarComponent::BeginPlay()
{
	Super::BeginPlay();

	DetectCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectCollision->SetCollisionObjectType(ECC_Visibility);	
	DetectCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectCollision->IgnoreActorWhenMoving(OwnerFighter, true);
	if (OwnerFighter->IsLocallyControlled())
	{
		DetectCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
		DetectCollision->SetGenerateOverlapEvents(true);
		DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectCollsionBeginOverlap);
		DetectCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectCollsionEndOverlap);
		DetectCollision->SetSphereRadius(MaximumRadarSearchRadius * 100.0f);
		DetectCollision->bHiddenInGame = true;
	}

	if (OwnerFighter->IsLocallyControlled())
	{
		FTimerHandle RadarInfoUpdate;
		GetWorld()->GetTimerManager().SetTimer(RadarInfoUpdate, this, &ThisClass::CheckCollisionList, CheckListFrequency, true);
	}
}

void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerFighter->IsLocallyControlled())
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
	}
}

void URadarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(URadarComponent, DetectedTargets, COND_OwnerOnly);
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
	UE_LOG(LogTemp, Warning, TEXT("Target detected"));
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
					break;
				}
			}
		}
	}
}

void URadarComponent::LocalNewTargetLost(AActor* Target)
{
	AFighter* FighterTarget = Cast<AFighter>(Target);
	if (FighterTarget)
	{
		SetFighterMarkState(FighterTarget, ETargetMarkState::Lost);
	}
}

void URadarComponent::SetFighterMarkState(AFighter* target, ETargetMarkState MarkState, float distance)
{
	UMarkWidget* MarkWidget = StaticCast<UMarkWidget*>(target->GetMarkWidget()->GetWidget());
	if (MarkWidget) {
		MarkWidget->SetMarkState(MarkState);
		MarkWidget->Distance = distance;
	}
}

void URadarComponent::CheckCollisionList()
{
	TSet<AActor*> otherActors;
	DetectCollision->GetOverlappingActors(otherActors);
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

	if(HitResult.bBlockingHit)
		UE_LOG(LogTemp, Warning, TEXT("Target block"));

	return HitResult.bBlockingHit;
}

void URadarComponent::DetectFighterOnRWSMode(AFighter* target)
{
	if (target->RWSDetectTimer <= 0.0f)
	{
		target->RWSDetectTimer = RWSScaningPeriod;
		SetFighterMarkState(target, ETargetMarkState::RWSEnemy);
	}
}

void URadarComponent::DetectFighterOnVTMode(AFighter* target)
{
	FVector TargetPos = target->GetActorLocation();
	FVector TargetRelativePos = OwnerFighter->GetTransform().InverseTransformPosition(TargetRelativePos);
	bool InVTScanRange = false;
	if (TargetRelativePos.X > 0)
	{
		float HAngle = FMath::Abs(FMath::RadiansToDegrees(FMath::Atan2(TargetRelativePos.Y, TargetRelativePos.X)));
		UE_LOG(LogTemp, Warning, TEXT("HAngle: %f"), HAngle);
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
	}
	else
	{
		SetFighterMarkState(target, ETargetMarkState::Lost);
	}
}

void URadarComponent::DetectFighterOnSTTMode(AFighter* target)
{

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
	CurrentRadarMode = CurrentRadarMode == ERadarMode::RWS ?
		ERadarMode::VT : ERadarMode::RWS;
}





