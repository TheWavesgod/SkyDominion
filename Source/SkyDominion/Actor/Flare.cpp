
#include "Flare.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AFlare::AFlare()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionShape"));
	SetRootComponent(CollisionShape);
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionShape->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);

	FlareVisionFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlareVisionFX")); 
	FlareVisionFX->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	Tags.AddUnique(FName("Flare"));
}

void AFlare::BeginPlay()
{
	Super::BeginPlay();

	HeatIndex = 45 + FMath::FRand() * 10.0f;

	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Initial Flare On Server"));
		CollisionShape->SetGenerateOverlapEvents(true);
		CollisionShape->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
		CollisionShape->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	}

	SetLifeSpan(8.0f);
}

void AFlare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AFlare::GetHeatIndex() const
{
	return HeatIndex;
}

