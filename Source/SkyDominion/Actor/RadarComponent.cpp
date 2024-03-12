// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "SkyDominion/HUD/MarkWidget.h"
#include "Components/WidgetComponent.h"

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
	if (OwnerFighter->GetLocalRole() == ROLE_Authority)
	{
		DetectCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
		DetectCollision->SetGenerateOverlapEvents(true);
		DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectCollsionBeginOverlap);
		DetectCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectCollsionEndOverlap);
		DetectCollision->SetSphereRadius(RadarSearchRadius * 100.0f);
		DetectCollision->bHiddenInGame = true;
	}

	if (OwnerFighter->IsLocallyControlled())
	{
		FTimerHandle RadarInfoUpdate;
		float UpdateFrequency = 1.0f / 60.0f;
		GetWorld()->GetTimerManager().SetTimer(RadarInfoUpdate, this, &ThisClass::LocalUpdateTargetsList, UpdateFrequency, true);
	}
}

void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerFighter->IsLocallyControlled())
	{
		/*TSet<AActor*> otherActors;
		DetectCollision->GetOverlappingActors(otherActors);*/
		if (OwnerFighter->HasAuthority())
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Sever: Detect Actor Num: %d"), DetectedTargets.Num()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Client: Detect Actor Num: %d"), DetectedTargets.Num()));
		}
	}
}

void URadarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URadarComponent, DetectedTargets, COND_OwnerOnly);
}

void URadarComponent::OnDetectCollsionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ServerUpdateTargetsList(OtherActor, true);
}

void URadarComponent::OnDetectCollsionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ServerUpdateTargetsList(OtherActor, false);
}

void URadarComponent::ServerUpdateTargetsList(AActor* Target, bool bIsNewTarget)
{
	if (bIsNewTarget)
	{			
		DetectedTargets.AddUnique(Target);
	}
	else
	{
		DetectedTargets.Remove(Target);
	}
}

void URadarComponent::LocalUpdateTargetsList()
{
	for (AActor* Target : DetectedTargets)
	{
		if (!DetectedTargetsInMemory.Contains(Target))
		{
			// New Target
			LocalNewTargetDetected(Target);
		}
	}

	for (AActor* Target : DetectedTargetsInMemory)
	{
		if (!DetectedTargets.Contains(Target))
		{
			// Target Lost
			LocalNewTargetLost(Target);
		}
	}

	DetectedTargetsInMemory = TSet<AActor*>(DetectedTargets);

	for (AActor* Target : DetectedTargetsInMemory)
	{
		LoaclTargetsInfoUpdate(Target);
	}
}

void URadarComponent::LocalNewTargetDetected(AActor* Target)
{
	AFighter* NewFighterTarget = Cast<AFighter>(Target);
	if (NewFighterTarget)
	{
		NewFighterTarget->SetMarkWidgetVisble(true);
		UMarkWidget* MarkWidget = Cast<UMarkWidget>(NewFighterTarget->GetMarkWidget()->GetWidget());
		if (OwnerFighter->bInRedTeam == NewFighterTarget->bInRedTeam)
		{
			if (MarkWidget) { MarkWidget->SetMarkState(ETargetMarkState::TeamMate); }
			NewFighterTarget->Tags.AddUnique("TeamMate");
		}
		else
		{
			if (MarkWidget) { MarkWidget->SetMarkState(ETargetMarkState::Enemy); }
			NewFighterTarget->Tags.AddUnique("Enemy");
		}
		NewFighterTarget->Tags.AddUnique("FighterJet");
	}
}

void URadarComponent::LocalNewTargetLost(AActor* Target)
{
	AFighter* NewFighterTarget = Cast<AFighter>(Target);
	if (NewFighterTarget)
	{
		NewFighterTarget->SetMarkWidgetVisble(false);
	}
}

void URadarComponent::LoaclTargetsInfoUpdate(AActor* Target)
{
	AFighter* NewFighterTarget = Cast<AFighter>(Target);
	if (NewFighterTarget)
	{
		UMarkWidget* MarkWidget = Cast<UMarkWidget>(NewFighterTarget->GetMarkWidget()->GetWidget());
		if (MarkWidget)
		{
			MarkWidget->Distance = FMath::Abs((NewFighterTarget->GetActorLocation() - OwnerFighter->GetActorLocation()).Size() / 100.0f);
		}
	}
}







