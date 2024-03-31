
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


AMissile::AMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	CollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionShape"));
	SetRootComponent(CollisionShape);
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionShape->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	MissileMesh->SetupAttachment(RootComponent);
	MissileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetAutoActivate(false);

	ThrusterFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterFX"));
	ThrusterFX->SetupAttachment(RootComponent);
	ThrusterFX->bAutoActivate = false;

	/*ExplosionFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionFX"));
	ExplosionFX->SetupAttachment(RootComponent);
	ExplosionFX->bAutoActivate = false;*/
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

	if (!FighterOnwer) return;

	// Record Missile's Velocity before fire;
	if (!bHasFired)
	{
		LastFrameLocation = CurrentFrameLocation;
		CurrentFrameLocation = GetActorLocation();
		MissileVelocity = (CurrentFrameLocation - LastFrameLocation) / DeltaTime * 0.01f;

		return;
	}

	if (HasAuthority())
		UpdateMissileMovement(DeltaTime);
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

	FVector lift = liftDir * AirdensityEffect * MissileRightArea * FMath::Square(sinAOA) * cosAOA;
	FVector drag = -MissileVel.GetSafeNormal() * AirdensityEffect * MissileRightArea * FMath::Cube(sinAOA);

	FVector forwardDrag = - 0.65f * 0.5f * 1.225f * FMath::Square(XSpeed) * MissileForwardArea * GetActorForwardVector();

	AeroForce = lift + drag + forwardDrag;
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
		bHasFired = true;

		CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionShape->SetGenerateOverlapEvents(true);
		CollisionShape->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		CollisionShape->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereCollisionBeginOverlap);
	}

}

void AMissile::OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == FighterOnwer) return;

	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Cyan, FString("Missile hit ") + OtherActor->GetName());

	if (ExplosionSound)
		UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, SweepResult.ImpactPoint, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f, ExplosionAttenuation);
	if (ExplosionFX)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionFX, SweepResult.Location, FRotator::ZeroRotator, FVector(ExplosionScale));

	Destroy();
}