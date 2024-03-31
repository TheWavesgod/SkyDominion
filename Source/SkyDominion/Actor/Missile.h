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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	FName MissileDisplayName;

	// Unit km
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MaxShootingRange = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MaxSpeedInMach = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float LaunchDelayTime = 0.0f;

	// Unit kg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MissileMass = 85.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MissileRightArea = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MissileForwardArea = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float ExplosionScale = 3.0f;

	UPROPERTY(Replicated)
	class AFighter* FighterOnwer;

	UPROPERTY()
	AActor* TrackTarget;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* CollisionShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* MissileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "General Settings")
	class UNiagaraComponent* ThrusterFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	class UNiagaraSystem* ExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	USoundCue* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	USoundAttenuation* ExplosionAttenuation;


	/** Missile State */
	bool bHasFired = false;

	/** Use for calculate missile movement */
	FVector MissileVelocity;

	FVector CurrentFrameLocation;
	FVector LastFrameLocation;

	float MaxThrusterForce;

	void CalculateMissileAeroForce(FVector MissileVel, FVector& AeroForce);

	UFUNCTION()
	virtual void OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void UpdateMissileMovement(float DeltaTime);

	UFUNCTION()
	void LaunchMissile();

public:	

};
