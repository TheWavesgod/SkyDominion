#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	CollisionBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionBody"));
	CollisionBody->SetupAttachment(RootComponent);
	CollisionBody->SetRelativeRotation(FQuat(FVector(0,1,0), FMath::DegreesToRadians(90.0f)));

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		CollisionBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionBody->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CollisionBody->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBody->SetCollisionResponseToChannel(ECC_Vehicle, ECollisionResponse::ECR_Overlap);
		CollisionBody->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
		CollisionBody->SetGenerateOverlapEvents(true);
		// CCD for fast move bullet
		CollisionBody->SetUseCCD(true);
		CollisionBody->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCollsionBeginOverlap);
	}
	else
	{
		CollisionBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(BulletLifeSpan);

	if (HasAuthority())
	{
		CollisionBody->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();
}

void AProjectile::OnCollsionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("Bullet Hit") + OtherActor->GetName());
	Destroy();
}

void AProjectile::SetVisualMeshVisibility(bool bIsVisible)
{
	VisualMesh->SetVisibility(bIsVisible);
}