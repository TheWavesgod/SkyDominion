
#include "Missile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AMissile::AMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	CollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionShape"));
	SetRootComponent(CollisionShape);
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	MissileMesh->SetupAttachment(RootComponent);
	MissileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThrusterFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterFX"));
	ThrusterFX->SetupAttachment(RootComponent);
	ThrusterFX->bAutoActivate = false;
}

void AMissile::BeginPlay()
{
	Super::BeginPlay();

	FighterOnwer = GetOwner<AFighter>();
}

void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FighterOnwer = FighterOnwer == nullptr ? GetOwner<AFighter>() : FighterOnwer;

	if (!FighterOnwer) return;

	if (!bHasFired)
	{
		LastFrameLocation = CurrentFrameLocation;
		CurrentFrameLocation = GetActorLocation();
		MissileVelocity = (CurrentFrameLocation - LastFrameLocation) / DeltaTime * 0.01f;
	}

	if (!bHasFired) return;

	UpdateMissileMovement(DeltaTime);
}


void AMissile::UpdateMissileMovement(float DeltaTime)
{
	MissileVelocity += GetActorForwardVector() * MaxThrusterForce * DeltaTime;

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Missile Speed: %d kmh"),FMath::RoundToInt(FMath::Abs(MissileVelocity.Size() * 3.6))));

	SetActorLocation(GetActorLocation() + MissileVelocity * 100.0f * DeltaTime, true);
}


void AMissile::Fire_Implementation()
{
	FighterOnwer = FighterOnwer == nullptr ? GetOwner<AFighter>() : FighterOnwer;

	if (!FighterOnwer) return;

	FTimerHandle LauchDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(LauchDelayHandle, this, &ThisClass::LaunchMissile, LaunchDelayTime, false);
}

void AMissile::LaunchMissile()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ThrusterFX->Activate(true);

	/*if (FighterOnwer->IsLocallyControlled())
	{
		UGameplayStatics::SpawnSound2D(this, FireSound);
	}*/

	UGameplayStatics::SpawnSound2D(this, FireSound);

	if (HasAuthority())
	{
		MissileVelocity += GetActorForwardVector() * MaxThrusterForce * 0.1f;

		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("Fighter Speed : %f"), MissileVelocity.Size() * 3.6));

		bHasFired = true;

		SetReplicatedMovement(FRepMovement());
	}

}

