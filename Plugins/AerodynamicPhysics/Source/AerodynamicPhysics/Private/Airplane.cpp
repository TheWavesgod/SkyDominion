#include "Airplane.h"
#include "Components/SkeletalMeshComponent.h"
#include "AeroPhysicsComponent.h"

AAirplane::AAirplane()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	AeroPhysicsComponent = CreateDefaultSubobject<UAeroPhysicsComponent>(TEXT("AeroPhysicsComponent"));
}

void AAirplane::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAirplane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAirplane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

