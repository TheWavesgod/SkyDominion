// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadarComponent.generated.h"

class AFighter;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYDOMINION_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URadarComponent();

	/** Settings for Radar */
	// unit meter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float RadarSearchRadius = 50000.0f;

	UPROPERTY(Replicated)
	TArray<AActor*> DetectedTargets;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDetectCollsionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectCollsionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ServerUpdateTargetsList(AActor* Target, bool bIsNewTarget);

	void LocalUpdateTargetsList();
	void LocalNewTargetDetected(AActor* Target);
	void LocalNewTargetLost(AActor* Target);
	void LoaclTargetsInfoUpdate(AActor* Target);
	TSet<AActor*> DetectedTargetsInMemory;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	AFighter* OwnerFighter;

	USphereComponent* DetectCollision;
		
};
