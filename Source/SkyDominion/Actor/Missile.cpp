
#include "Missile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Flare.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SkyDominion/Actor/RadarComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/WidgetComponent.h"
#include "SkyDominion/HUD/MarkWidget.h"
#include "Engine.h"


AMissile::AMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	CollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionShape"));
	SetRootComponent(CollisionShape);
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionShape->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	MissileMesh->SetupAttachment(RootComponent);
	MissileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetAutoActivate(false);

	MarkWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MarkWidget"));
	MarkWidgetComponent->SetupAttachment(RootComponent);
	MarkWidgetComponent->SetVisibility(false);

	ThrusterFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterFX"));
	ThrusterFX->SetupAttachment(RootComponent);
	ThrusterFX->bAutoActivate = false;
	
	Tags.AddUnique(FName("Missile"));
}

void AMissile::BeginPlay()
{
	Super::BeginPlay();

	CollisionShape->IgnoreActorWhenMoving(FighterOnwer, true);
	MaxThrusterForce = 0.65f * 0.5f * 1.225f * FMath::Square(MaxSpeedInMach * 340.29f) * MissileForwardArea;
}

void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (HasAuthority()) FighterOnwer = FighterOnwer == nullptr ? GetOwner<AFighter>() : FighterOnwer;

	// Record Missile's Velocity before fire;
	if (!bHasFired)
	{
		if (!FighterOnwer) return;
		this->bInRedTeam = FighterOnwer->bInRedTeam;
		if (HasAuthority())
		{
			OwnerController = OwnerController == nullptr ? FighterOnwer->Controller.Get() : OwnerController;
		}

		LastFrameLocation = CurrentFrameLocation;
		CurrentFrameLocation = GetActorLocation();
		MissileVelocity = (CurrentFrameLocation - LastFrameLocation) / DeltaTime * 0.01f;

		return;
	}

	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 10000.0f, FColor::Red, true);	

	if (HasAuthority())
	{
		CheckTrackTarget(DeltaTime);
		SendWarningToTarget(DeltaTime);
		CaculateRotationToTrackTarget(DeltaTime);
		UpdateMissileMovement(DeltaTime);
	}
}

void AMissile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMissile, FighterOnwer);
}


void AMissile::CheckTrackTarget(float DeltaTime)
{
	TrackCheckHandle = FMath::Clamp(TrackCheckHandle - DeltaTime, 0.0f, TrackCheckGap);
	if (TrackCheckHandle != 0.0f) return;

	TrackCheckHandle = TrackCheckGap;

	if (bUseInfraredTrack)
	{
		InfraredCheck();
	}
	else
	{
		SemiActiveCheck();
	}
}

void AMissile::InfraredCheck()
{
	if (IsValid(TrackTarget))
	{
		float TargetDis = (TrackTarget->GetActorLocation() - GetActorLocation()).Size() * 0.01;
		if (TargetDis > InfaredSearchRadius)
		{
			return;
		}
	}

	//int FlareNum = 0;
	//for (TActorIterator<AFlare> FlareItr(GetWorld()); FlareItr; ++FlareItr)
	//{
	//	FVector FlareLoc = FlareItr->GetActorLocation();
	//	float DistSqaured = FVector::DistSquared(FlareLoc, GetActorLocation());
	//	if (/*DistSqaured < FMath::Square(InfaredSearchRadius * 100.0f)*/1)
	//	{
	//		++FlareNum;
	//	}
	//}
	//UE_LOG(LogTemp, Warning, TEXT("Flare Num: %d"), FlareNum);

	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;	
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Vehicle));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this); 
	if (IsValid(FighterOnwer)) { IgnoreActors.Add(FighterOnwer); }
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), InfaredSearchRadius * 100.0f, ObjectTypes, nullptr, IgnoreActors, OverlappedActors);

	if (OverlappedActors.IsEmpty()) { return; }

	/*Algo::Sort(OverlappedActors,
		[&](const AActor* A, const AActor* B)
		{
			const IRadarInterface* IA = Cast<IRadarInterface>(A);
			const IRadarInterface* IB = Cast<IRadarInterface>(B);

			if (IA && IB)
			{
				return (IA->GetHeatIndex() > IB->GetHeatIndex());
			}

			return (IA != nullptr);
		}
	);*/
	AActor* NewTarget = nullptr;
	float HighestHeatIndex = 0.0f;
	for (auto i : OverlappedActors)
	{
		IRadarInterface* Ii = Cast<IRadarInterface>(i);
		if (!Ii) continue;
		if (i->ActorHasTag("Missile"))
		{
			AMissile* missile = StaticCast<AMissile*>(i);
			if (missile->bInRedTeam == this->bInRedTeam)
			{
				continue;
			}
		}
		if (!CheckTargetInInfaredSearchRange(i)) continue;
		if (Ii->GetHeatIndex() > HighestHeatIndex)
		{
			HighestHeatIndex = Ii->GetHeatIndex();
			NewTarget = i;
		}
	}

	if (NewTarget == nullptr || HighestHeatIndex == 0.0f) return;

	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Highest ") + TrackTarget->GetName());

	if (!IsValid(TrackTarget))
	{
		TrackTarget = NewTarget;
		return;
	}

	float CurrentTrackTragetHeatIndex = 0.0f;
	IRadarInterface* TrackTargetIF = Cast<IRadarInterface>(TrackTarget);
	if (TrackTargetIF)
	{
		CurrentTrackTragetHeatIndex = TrackTargetIF->GetHeatIndex();
	}

	float TargetChooseRandRatio = FMath::FRand();

	float CurrentTrackTragetRatio = CurrentTrackTragetHeatIndex / (CurrentTrackTragetHeatIndex + HighestHeatIndex);

	if (TargetChooseRandRatio > CurrentTrackTragetRatio)
	{
		TrackTarget = NewTarget;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Missile track: ") + TrackTarget->GetName());	
}

void AMissile::SemiActiveCheck()
{
	AActor* RadarTarget = FighterOnwer->GetRadarComponent()->GetLockedTarget();

	if (!IsValid(RadarTarget))
	{
		RadarTargetLostHandle = FMath::Clamp(RadarTargetLostHandle - TrackCheckGap, 0.0f, RadarTargetLostGap);
		if (RadarTargetLostHandle == 0.0f)
		{
			TrackTarget = nullptr;
		}
		return;
	}

	TrackTarget = RadarTarget;

	RadarTargetLostHandle = RadarTargetLostGap;

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("Missile track: ") + TrackTarget->GetName());

}

void AMissile::CalculateMissileAeroForce(FVector MissileVel, FVector& AeroForce)
{
	FVector RelativeVel = GetTransform().InverseTransformVector(MissileVel);
	
	float Speed = RelativeVel.Size();

	if (Speed == 0) 
	{
		AeroForce = FVector::ZeroVector;
		return;
	}

	float XSpeed = RelativeVel.X;
	float YSpeed = FVector(0.0f, RelativeVel.Y, RelativeVel.Z).Size();

	float sinAOA = YSpeed / Speed;
	float cosAOA = XSpeed / Speed;

	float AirdensityEffect = 0.5f * 1.225f * Speed * Speed;

	FVector liftDir = FVector::VectorPlaneProject(GetActorForwardVector(), MissileVel.GetSafeNormal()).GetSafeNormal();

	FVector lift = liftDir * AirdensityEffect * MissileRightArea * FMath::Square(sinAOA) * cosAOA * 30.0f;
	FVector drag = -MissileVel.GetSafeNormal() * AirdensityEffect * MissileRightArea * FMath::Cube(sinAOA);

	float DragDirRatio = XSpeed > 0.0f ? 1.0f : -1.0f;
	FVector forwardDrag = - 0.65f * 0.5f * 1.225f * FMath::Square(XSpeed) * MissileForwardArea * DragDirRatio * GetActorForwardVector();

	AeroForce = lift + drag + forwardDrag;
}

void AMissile::CaculateRotationToTrackTarget(float DeltaTime)
{
	TargetRotator = GetActorRotation();
	
	if (IsValid(TrackTarget))
	{
		FVector TargetLoc = TrackTarget->GetActorLocation();
		FVector TargetVelocity = TrackTarget->GetVelocity();

		FVector TargetDir = TargetLoc - GetActorLocation();
		
		float x = 0;
		if (TargetVelocity.Size() != 0)
		{
			float speedRatio = MissileVelocity.Size() / (TargetVelocity * 0.01f).Size();

			float cosTargetVelTargetDir = TargetLoc.GetSafeNormal().Dot(-TargetDir.GetSafeNormal());

			float a = speedRatio * speedRatio - 1;
			float b = 2 * TargetDir.Size() * 0.01f * cosTargetVelTargetDir;
			float c = -FMath::Square(TargetDir.Size() * 0.01f);

			if (a == 0)
			{
				x = cosTargetVelTargetDir > 0 ? TargetDir.Size() * 0.5f / cosTargetVelTargetDir : 0;
			}
			else
			{
				float discriminant = FMath::Square(b) - 4 * a * c;

				if (discriminant > 0)
				{
					float x1 = (-b + FMath::Sqrt(discriminant)) / (2 * a);
					float x2 = (-b - FMath::Sqrt(discriminant)) / (2 * a);

					if (cosTargetVelTargetDir > 0) // The Angle between target velocity and distance direction is less than 90 degree
					{
						x = x1 > 0 ? x1 : 0;
					}
					else if (cosTargetVelTargetDir == 0)
					{
						x = a > 0 ? FMath::Sqrt(-c / a) : 0;
					}
					else
					{
						if (a > 0)
						{
							x = x1 > 0 ? x1 : 0;
						}
					}
				}
			}
		}

		FVector TrackLoc = TargetLoc + TargetVelocity.GetSafeNormal() * x * 200.0f;

		//DrawDebugLine(GetWorld(), TrackLoc, TrackLoc + FVector(0,0,1) * 1000.0f, FColor::Red, false, 1.0f);

		TargetRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLoc);
	}

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotator, DeltaTime, 15.0f));
}

void AMissile::SendWarningToTarget(float DeltaTime)
{
	WarningTimeHandle = FMath::Clamp(WarningTimeHandle - DeltaTime, 0.0f, WarningTimeGap);

	if (!IsValid(TrackTarget)) return;

	if (WarningTimeHandle > 0.0f) return;

	WarningTimeHandle = WarningTimeGap;

	IRadarInterface* TargetIf = Cast<IRadarInterface>(TrackTarget);

	bool bIsClose = (TrackTarget->GetActorLocation() - GetActorLocation()).Size() * 0.01f < 2000.0f ;

	if (TargetIf)
		TargetIf->MissileComingWarning(bIsClose);
}

void AMissile::UpdateMissileMovement(float DeltaTime)
{
	FVector CurrentAeroForce; 
	CalculateMissileAeroForce(MissileVelocity, CurrentAeroForce);

	FVector ThrusterForce = GetActorForwardVector() * MaxThrusterForce;

	FVector CurrentFrameForce = ThrusterForce + CurrentAeroForce;

	MissileVelocity += CurrentFrameForce / MissileMass * DeltaTime;

	ProjectileMovementComponent->Velocity = MissileVelocity * 100.0f;
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Missile Speed: %d kmh"),FMath::RoundToInt(FMath::Abs(MissileVelocity.Size() * 3.6))));
}


void AMissile::Fire_Implementation()
{
	//if (HasAuthority()) FighterOnwer = FighterOnwer == nullptr ? GetOwner<AFighter>() : FighterOnwer;

	if (!FighterOnwer) return;

	if (FighterOnwer->IsLocallyControlled())
	{
		UGameplayStatics::SpawnSound2D(this, FireSound);
	}

	if (LaunchDelayTime == 0.0f)
	{
		LaunchMissile();
		return;
	}

	FTimerHandle LauchDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(LauchDelayHandle, this, &ThisClass::LaunchMissile, LaunchDelayTime, false);
}

void AMissile::LaunchMissile()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ThrusterFX->Activate(true);

	HeatIndex = 50.0f + (MaxSpeedInMach - 3.5) / 3.5 * 10.0f;

	ProjectileMovementComponent->SetActive(true);

	CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionShape->SetGenerateOverlapEvents(true);
	CollisionShape->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	if (HasAuthority())
	{
		AActor* RadarTarget = FighterOnwer->GetRadarComponent()->GetLockedTarget();
		if (IsValid(RadarTarget))
		{
			TrackTarget = FighterOnwer->GetRadarComponent()->GetLockedTarget();
			//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Missile track: ") + TrackTarget->GetName());
		}

		MissileVelocity += GetActorForwardVector() * MaxThrusterForce / MissileMass * 0.1f;
		ProjectileMovementComponent->Velocity = MissileVelocity * 100.0f;

		CollisionShape->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		CollisionShape->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereCollisionBeginOverlap);

		SetLifeSpan(60.0f);
	}

	bHasFired = true;
}

void AMissile::OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(FighterOnwer))
	{
		if (OtherActor == FighterOnwer) return;
	}

	if (OtherActor->ActorHasTag("Flare"))
	{
		if (OtherActor != TrackTarget)
		{
			return;
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Cyan, FString("Missile hit ") + OtherActor->GetName());

	SpawnExplosionFX(SweepResult.Location);

	UGameplayStatics::ApplyDamage(OtherActor, 200.0f, OwnerController, this, UDamageType::StaticClass());
	TArray<AActor*> ignoredActors;
	UGameplayStatics::ApplyRadialDamage(this, 500.0f, SweepResult.Location, 10000.0f, UDamageType::StaticClass(), ignoredActors, this, OwnerController);

	Destroy();
}

void AMissile::SpawnExplosionFX_Implementation(FVector Location)
{
	if (ExplosionSound)
		UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, Location, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f, ExplosionAttenuation);
	if (ExplosionFX)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionFX, Location, FRotator::ZeroRotator, FVector(ExplosionScale));
}

void AMissile::SetMissileMarkVisbility(bool bVisible, bool bIsSameTeam)
{
	MarkWidget = MarkWidget == nullptr ? StaticCast<UMarkWidget*>(MarkWidgetComponent->GetWidget()) : MarkWidget;

	MarkWidgetComponent->SetVisibility(bVisible);

	if (!bVisible) return;
	if (MarkWidget) 
	{ 
		MarkWidget->SetMissileMarkState(bIsSameTeam); 
		if (bIsSameTeam)
		{
			MarkWidget->Name = MissileDisplayName.ToString();
		}
	}
}

bool AMissile::CheckTargetInInfaredSearchRange(const AActor* target)
{
	if (!target) return false;

	FVector targetDir = (target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	float cosTargetDir = targetDir.Dot(GetActorForwardVector());

	return cosTargetDir < 0.5f;
}

float AMissile::GetHeatIndex() const
{
	return HeatIndex;
}
