#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class SKYDOMINION_API AMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissile();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	FName MissileDisplayName;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* CollisionShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* MissileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UNiagaraComponent* ThrusterFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USoundCue* FireSound;

	/** Use for calculate missile movement */
	FVector MissileVelocity;

public:	

};
