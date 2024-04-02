#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyDominion/Interface/RadarInterface.h"

#include "Missile.generated.h"



UCLASS()
class SKYDOMINION_API AMissile : public AActor, public IRadarInterface
{
	GENERATED_BODY()
	
public:	
	AMissile();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Fire();

	UFUNCTION(NetMulticast, Reliable)
	virtual void SpawnExplosionFX(FVector Location);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	FName MissileDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	bool bUseInfraredTrack = false;

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

	bool bInRedTeam = true;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* CollisionShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* MissileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UWidgetComponent* MarkWidgetComponent;

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

	UPROPERTY()
	class UMarkWidget* MarkWidget;

	AController* OwnerController;

	/** Missile State */
	bool bHasFired = false;

	/** Use for calculate missile movement */
	FVector MissileVelocity;
	FRotator TargetRotator;

	FVector CurrentFrameLocation;
	FVector LastFrameLocation;

	float MaxThrusterForce;

	void CalculateMissileAeroForce(FVector MissileVel, FVector& AeroForce);

	/** Use for send track target missile Warning */
	const float WarningTimeGap = 0.3f;
	float WarningTimeHandle = 0.0f;

	/** Use for track target Check */
	const float TrackCheckGap = 0.2f;
	float TrackCheckHandle = 0.0f;

	// Infrared Missile Check
	void InfraredCheck();
	bool CheckTargetInInfaredSearchRange(const AActor* target);
	float InfaredSearchRadius = 3000.0f; // unit m
	float HeatIndex = 0.0f;

	// Semi-active radar Mode
	void SemiActiveCheck();
	const float RadarTargetLostGap = 0.8f;
	float RadarTargetLostHandle = 0.0f;

	UFUNCTION()
	virtual void OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void CheckTrackTarget(float DeltaTime);

	void UpdateMissileMovement(float DeltaTime);

	void CaculateRotationToTrackTarget(float DeltaTime);

	void SendWarningToTarget(float DeltaTime);

	UFUNCTION()
	void LaunchMissile();

public:	
	void SetMissileMarkVisbility(bool bVisible, bool bIsSameTeam);

	FORCEINLINE bool GetMissileHasFired() const { return bHasFired; }

	virtual float GetHeatIndex() const override;

};
