
#include "Missile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"

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
	
}

void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMissile::Fire()
{
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}

