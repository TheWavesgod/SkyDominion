#include "Projectile.h"
#include "Components/CapsuleComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	CollisionBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionBody"));
	CollisionBody->SetupAttachment(RootComponent);
	CollisionBody->SetRelativeRotation(FQuat(FVector(), FMath::));

	if (HasAuthority())
	{
		CollisionBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionBody->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CollisionBody->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBody->SetCollisionResponseToChannel(ECC_Vehicle, ECollisionResponse::ECR_Overlap);
		CollisionBody->SetGenerateOverlapEvents(true);
	}
	else
	{
		CollisionBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

