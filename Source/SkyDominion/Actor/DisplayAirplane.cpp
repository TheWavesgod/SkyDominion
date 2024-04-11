

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
		AssetPath = TEXT("SkeletalMesh'/Game/Assets/VigilanteContent/Vehicles/West_Fighter_F15E/SK_West_Fighter_F15E.SK_West_Fighter_F15E'"); 
		break;

	case EFighterJetType::E_Su33:
		AssetPath = TEXT("SkeletalMesh'/Game/Assets/VigilanteContent/Vehicles/East_Fighter_Su33/SK_East_Fighter_Su33.SK_East_Fighter_Su33'"); 
		break;

	case EFighterJetType::E_F18C:
		AssetPath = TEXT("SkeletalMesh'/Game/Assets/VigilanteContent/Vehicles/West_Fighter_F18C/SK_West_Fighter_F18C.SK_West_Fighter_F18C'"); 
		break;

	case EFighterJetType::E_Su24:
		AssetPath = TEXT("SkeletalMesh'/Game/Assets/VigilanteContent/Vehicles/East_Bomber_SU24/SK_East_Bomber_SU24.SK_East_Bomber_SU24'"); 
		break;

	case EFighterJetType::E_Rafale:
		AssetPath = TEXT("SkeletalMesh'/Game/VigilanteMovement/Vehicles/West_Fighter_Rafale/SK_West_Fighter_Rafale.SK_West_Fighter_Rafale'");
		break; 

	case EFighterJetType::E_Mig29:
		AssetPath = TEXT("SkeletalMesh'/Game/VigilanteMovement/Vehicles/East_Fighter_MiG29/SK_East_Fighter_MiG29.SK_East_Fighter_MiG29'"); 
		break;

	case EFighterJetType::E_F16C:
		AssetPath = TEXT("SkeletalMesh'/Game/VigilanteMovement/Vehicles/West_Fighter_F16C/SK_West_Fighter_F16C.SK_West_Fighter_F16C'"); 
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

