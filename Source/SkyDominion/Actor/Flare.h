#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyDominion/Interface/RadarInterface.h"

#include "Flare.generated.h"

UCLASS()
class SKYDOMINION_API AFlare : public AActor, public IRadarInterface
{
	GENERATED_BODY()
	
public:	
	AFlare();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* CollisionShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UNiagaraComponent* FlareVisionFX;

	float HeatIndex = 0.0f;

public:	
	virtual float GetHeatIndex() const override;
};
