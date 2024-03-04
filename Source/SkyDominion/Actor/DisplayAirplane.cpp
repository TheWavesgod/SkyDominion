

#include "DisplayAirplane.h"
#include "Components/SkeletalMeshComponent.h"

ADisplayAirplane::ADisplayAirplane()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
}

void ADisplayAirplane::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADisplayAirplane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

