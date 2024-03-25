

#include "DisplayAirplane.h"
#include "Components/SkeletalMeshComponent.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "UObject/ConstructorHelpers.h"

ADisplayAirplane::ADisplayAirplane()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
}

void ADisplayAirplane::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADisplayAirplane::SetDisplayPlaneByIndex(const int Index)
{
	FString AssetPath;
	switch (static_cast<EFighterJetType>(Index))
	{
	case EFighterJetType::E_F15E:
		AssetPath = TEXT("SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Fighter_F15E/SK_West_Fighter_F15E.SK_West_Fighter_F15E'"); 
		break;

	case EFighterJetType::E_Su33:
		AssetPath = TEXT("SkeletalMesh'/Game/VigilanteContent/Vehicles/East_Fighter_Su33/SK_East_Fighter_Su33.SK_East_Fighter_Su33'"); 
		break;

	case EFighterJetType::E_F18C:
		AssetPath = TEXT("SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Fighter_F18C/SK_West_Fighter_F18C.SK_West_Fighter_F18C'"); 
		break;
	}
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *AssetPath);
	if (SkeletalMesh)
	{
		Mesh->SetSkeletalMesh(SkeletalMesh);
	}
}

void ADisplayAirplane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

