
#include "Missile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SkyDominion/Actor/RadarComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "SkyDominion/Interface/RadarInterface.h"
#include "Components/WidgetComponent.h"
#include "SkyDominion/HUD/MarkWidget.h"


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

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 10000.0f, FColor::Red, true);	

	if (HasAuthority())
	{
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

	FVector lift = liftDir * AirdensityEffect * MissileRightArea * FMath::Square(sinAOA) * cosAOA * 10.0f;
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

		float TargetDis = (TargetLoc - GetActorLocation()).Size();
		UE_LOG(LogTemp, Warning, TEXT("Missile Target Dis: %f"), TargetDis * 0.01f);

		float TrackTime = 0.0f;

		float DirectSpeed = MissileVelocity.Dot((TargetLoc - GetActorLocation()).GetSafeNormal());

		TrackTime = DirectSpeed > 0 ? TargetDis * 0.01 / DirectSpeed : 0.0f;

		FVector TrackLoc = TargetLoc + TargetVelocity * TrackTime * 10.0f;

		DrawDebugLine(GetWorld(), TrackLoc, TrackLoc + FVector(0,0,1) * 1000.0f, FColor::Red, false, 1.0f);

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

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Missile Speed: %d kmh"),FMath::RoundToInt(FMath::Abs(MissileVelocity.Size() * 3.6))));
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
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Missile track: ") + TrackTarget->GetName());
		}

		MissileVelocity += GetActorForwardVector() * MaxThrusterForce / MissileMass * 0.1f;
		ProjectileMovementComponent->Velocity = MissileVelocity * 100.0f;

		CollisionShape->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		CollisionShape->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereCollisionBeginOverlap);
	}

	bHasFired = true;
}

void AMissile::OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(FighterOnwer))
	{
		if (OtherActor == FighterOnwer) return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Cyan, FString("Missile hit ") + OtherActor->GetName());

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