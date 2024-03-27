
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MissileComponent.generated.h"

USTRUCT(BlueprintType)
struct FMissileConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMissile> MissileClass;

	UPROPERTY(EditAnywhere)
	FVector AttachOffset;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYDOMINION_API UMissileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMissileComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Set Missile for every socket like Missile_1 ~ Missile_8
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	TArray<FMissileConfig> MissileConfig;

	void SpawnMissileAtBegin(class USceneComponent* parent);

	void DestroyMissileRemain();

	void ChangeSelectMissile();

	void FireCurrentMissile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	class AFighter* OwnerFighter;

	UPROPERTY()
	TArray<class AMissile*> MissilesUnfire;  

	UPROPERTY()
	TArray<FName> MissileType;

	UPROPERTY()
	TMap<FName, int32> CurrentMissileNum;

private:
	FName GetMissileSocketNameByIndex(int index) const;

	AMissile* GetFirstMissileFromType(FName TargetMissileType);

	UPROPERTY()
	int32 CurrentSelectMissileTypeIndex = 0;

	UPROPERTY()
	AMissile* CurrentSelectMissile;

public:
	FName GetSelectMissileName() const;
	int GetSelectMissileNum() const;
};
