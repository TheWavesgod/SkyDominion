
#include "MissileComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Missile.h"


UMissileComponent::UMissileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwnerFighter = GetOwner<AFighter>();
}


void UMissileComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMissileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMissileComponent::SpawnMissileAtBegin(USceneComponent* parent)
{
	if (!OwnerFighter) return;

	MissilesUnfire.Empty();
	for (int i = 0; i < MissileConfig.Num(); ++i)
	{
		if (!MissileConfig[i].MissileClass) continue;

		AMissile* SpawnMissile = GetWorld()->SpawnActor<AMissile>(MissileConfig[i].MissileClass, FVector::ZeroVector, FRotator::ZeroRotator);

		MissilesUnfire.Emplace(nullptr);
		if (SpawnMissile)
		{
			SpawnMissile->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetIncludingScale, GetMissileSocketNameByIndex(i));
			SpawnMissile->SetActorRelativeLocation(MissileConfig[i].AttachOffset);
			SpawnMissile->SetOwner(OwnerFighter);

			MissilesUnfire[i] = SpawnMissile;
			MissileType.Find(SpawnMissile->MissileDisplayName) == INDEX_NONE ? MissileType.Add(SpawnMissile->MissileDisplayName) : 0;
			if (!CurrentMissileNum.Find(SpawnMissile->MissileDisplayName))
			{
				CurrentMissileNum.Add(SpawnMissile->MissileDisplayName, 1);
			}
			else
			{
				*CurrentMissileNum.Find(SpawnMissile->MissileDisplayName) += 1;
			}
		}
	}

	if (!MissileType.IsEmpty())
	{
		CurrentSelectMissile = GetFirstMissileFromType(GetSelectMissileName());
	}
}

void UMissileComponent::DestroyMissileRemain()
{
	for (auto i : MissilesUnfire)
	{
		if (!i) continue;
		i->Destroy();
	}
}

void UMissileComponent::ChangeSelectMissile()
{
	CurrentSelectMissileTypeIndex = CurrentSelectMissileTypeIndex + 1 >= MissileType.Num() ? 0 : CurrentSelectMissileTypeIndex + 1;

	CurrentSelectMissile = GetFirstMissileFromType(GetSelectMissileName());

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Select Missile ") + GetSelectMissileName().ToString() + FString::Printf(TEXT(" Num: %d"), GetSelectMissileNum()));
}	

FName UMissileComponent::GetMissileSocketNameByIndex(int index) const
{
	FString MissileSocket = FString("Missile_") + FString::FromInt(index + 1);

	return FName(MissileSocket);
}

AMissile* UMissileComponent::GetFirstMissileFromType(FName TargetMissileType)
{
	if(MissilesUnfire.IsEmpty()) return nullptr;

	if (GetSelectMissileNum() == 0) return nullptr;

	for (auto i : MissilesUnfire)
	{
		if(i == nullptr) continue;

		if (i->MissileDisplayName.IsEqual(TargetMissileType))
		{
			return i;
		}
	}
	return nullptr;
}

FName UMissileComponent::GetSelectMissileName() const
{
	if (CurrentSelectMissileTypeIndex >= MissileType.Num()) return FName();

	return MissileType[CurrentSelectMissileTypeIndex];
}

int UMissileComponent::GetSelectMissileNum() const
{
	return CurrentMissileNum.Find(GetSelectMissileName()) ? *CurrentMissileNum.Find(GetSelectMissileName()) : 0;
}

