#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "AutoCannon.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CollisionBody = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionBody"));
	SetRootComponent(CollisionBody);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetCollisionObjectType(ECC_WorldDynamic);
	VisualMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	VisualMesh->SetupAttachment(CollisionBody);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 100000.0f;

	if (HasAuthority())
	{
		CollisionBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionBody->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CollisionBody->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBody->SetCollisionResponseToChannel(ECC_Vehicle, ECollisionResponse::ECR_Overlap);
		CollisionBody->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
		CollisionBody->SetGenerateOverlapEvents(true);
		// CCD for fast move bullet
		//CollisionBody->SetUseCCD(true);
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
		//AFighter* fighter = GetOwner<AFighter>();
		//ProjectileMovementComponent->InitialSpeed = fighter->GetAutoCannon()->InitSpeed * 100.0f;
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
	if (HasAuthority())
	{
		AFighter* OwnerFighter = GetOwner<AFighter>();
		if (OwnerFighter && OwnerFighter->Controller)
		{
			UGameplayStatics::ApplyDamage(OtherActor, 10.0f, OwnerFighter->Controller, this, UDamageType::StaticClass());
		}
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("Bullet Hit By") + GetOwner()->GetName());	
	}
	Destroy();
}

void AProjectile::SetVisualMeshVisibility(bool bIsVisible)
{
	VisualMesh->SetVisibility(bIsVisible);
}