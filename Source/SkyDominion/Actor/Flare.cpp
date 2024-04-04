
#include "Flare.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AFlare::AFlare()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionShape"));
	SetRootComponent(CollisionShape);
	CollisionShape->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionShape->SetGenerateOverlapEvents(true);

	FlareVisionFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlareVisionFX")); 
	FlareVisionFX->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	Tags.AddUnique(FName("Flare"));
}

void AFlare::BeginPlay()
{
	Super::BeginPlay();

	HeatIndex = 45 + FMath::FRand() * 10.0f;

	CollisionShape->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	CollisionShape->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
	CollisionShape->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	CollisionShape->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Initial Flare On Server"));
		CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereCollisionBeginOverlap);
		CollisionShape->SetGenerateOverlapEvents(true);
	}
	SetLifeSpan(8.0f);
}

void AFlare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlare::OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

float AFlare::GetHeatIndex() const
{
	return HeatIndex;
}

