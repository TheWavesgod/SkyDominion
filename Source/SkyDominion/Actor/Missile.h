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

	UFUNCTION(NetMulticast, Reliable)
	virtual void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	FName MissileDisplayName;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* CollisionShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* MissileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "General Settings")
	class UNiagaraComponent* ThrusterFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	class USoundCue* FireSound;

	UPROPERTY()
	class AFighter* FighterOnwer;

	/** Missile State */
	bool bHasFired = false;

	/** Use for calculate missile movement */
	FVector MissileVelocity;

	FVector CurrentFrameLocation;
	FVector LastFrameLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MaxThrusterForce = 10.0f;

private:
	void UpdateMissileMovement(float DeltaTime);

public:	

};
