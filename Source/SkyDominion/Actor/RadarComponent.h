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
	float RWSScaningPeriod = 5.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Radar Config")
	float VTModeScanAngle = 42.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDetectCollsionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectCollsionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void LocalNewTargetDetected(AActor* Target);
	void LocalNewTargetLost(AActor* Target);

	void SetFighterMarkState(AFighter* target, ETargetMarkState MarkState, float distance = 0.0f);

	/** RadarMode */
	ERadarMode CurrentRadarMode = ERadarMode::RWS;

private:	
	/** Determine collision actor can be search by Radar */
	void CheckCollisionList();
	float CheckListFrequency = 1 / 60.0f;

	bool CheckCollisionBetweenTargetAndSelf(const AActor* target);

	void DetectFighterOnRWSMode(AFighter* target);

	void DetectFighterOnVTMode(AFighter* target);

	void DetectFighterOnSTTMode(AFighter* target);

public:	
	AFighter* OwnerFighter;

	USphereComponent* DetectCollision;

	FString GetCurrentRadarMode() const;
	FORCEINLINE ERadarMode GetRadarModeEnum() const { return CurrentRadarMode; }

	void ChangeRadarMode();
};
