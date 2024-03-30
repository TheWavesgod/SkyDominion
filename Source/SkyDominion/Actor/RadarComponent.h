// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyEnum.h"

#include "RadarComponent.generated.h"

class AFighter;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYDOMINION_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URadarComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/** Settings for Radar */
	// unit meter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Radar Config")
	float MaximumRadarSearchRadius = 50000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Radar Config")
	float RWSScaningPeriod = 8.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Radar Config")
	float VTModeScanAngle = 42.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Radar Config")
	float STTModeScanAngle = 120.0f;

	// How much time needed for lock a target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Radar Config")
	float TimeToLocked = 2.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDetectCollsionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectCollsionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void LocalNewTargetDetected(AActor* Target);
	void LocalNewTargetLost(AActor* Target);

	void SetFighterMarkState(AFighter* target, ETargetMarkState MarkState, float distance = 0.0f);

	UFUNCTION(Server, Reliable)
	void ActiveTargetRWSScanedAlert(AFighter* target);

	UFUNCTION(Server, Reliable)
	void ActiveTargetVTScanedAlert(AFighter* target);

	UFUNCTION(Server, Reliable)
	void ActiveTargetSTTLockedAlert(AFighter* target);

	UFUNCTION(Server, Reliable)
	void DeactiveTargetSTTLockedAlert(AFighter* target);

	/** RadarMode */
	ERadarMode CurrentRadarMode = ERadarMode::RWS;

	/** Lock Mechanism */
	UPROPERTY();
	TArray<AFighter*> EnemyFightersDetected;

	int32 LockTargetIndex = 0;

	float LockingTimeHandle = 0.0f;

	void CheckLockState(float DeltaTime);

	UPROPERTY()
	AFighter* TargetBeingLocked;

	
	void AddDetectedEnemy(AFighter* Target);
	void DetectedEnemyLost(AFighter* Target);

private:	
	/** Determine collision actor can be search by Radar */
	void CheckCollisionList();
	float CheckListFrequency = 1 / 60.0f;

	bool CheckCollisionBetweenTargetAndSelf(const AActor* target);

	void DetectFighterOnRWSMode(AFighter* target);

	void DetectFighterOnVTMode(AFighter* target);

	void DetectFighterOnSTTMode(AFighter* target);


	bool bRadarBeInitiated = false;
	void InitiateRadar();

public:	

	AFighter* OwnerFighter;

	USphereComponent* DetectCollision;

	FString GetCurrentRadarMode() const;
	FORCEINLINE ERadarMode GetRadarModeEnum() const { return CurrentRadarMode; }

	void ChangeRadarMode();

	void StartLockTarget();
};
