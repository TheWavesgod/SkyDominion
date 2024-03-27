
#include "Missile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

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

	if (!bHasFired) return;

	UpdateMissileMovement(DeltaTime);
}


void AMissile::UpdateMissileMovement(float DeltaTime)
{
	MissileVelocity += GetActorForwardVector() * MaxThrusterForce * DeltaTime;

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Missile Speed: %d kmh"), FMath::Abs(MissileVelocity.Size() * 3.6)));

	SetActorLocation(GetActorLocation() + MissileVelocity * 100.0f * DeltaTime, true);
}

void AMissile::Fire_Implementation()
{
	FighterOnwer = FighterOnwer == nullptr ? GetOwner<AFighter>() : FighterOnwer;

	if (!FighterOnwer) return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ThrusterFX->Activate(true);

	UGameplayStatics::SpawnSound2D(this, FireSound);

	if (HasAuthority())
	{
		MissileVelocity = FighterOnwer->GetMesh()->GetPhysicsLinearVelocity() / 100.0f;

		bHasFired = true;

		SetReplicatedMovement(FRepMovement());
	}
	
}

