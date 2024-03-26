// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FighterWreckage.generated.h"

UCLASS()
class SKYDOMINION_API AFighterWreckage : public AActor
{
	GENERATED_BODY()
	
public:	
	AFighterWreckage();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wreckage Config")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wreckage Config")
	float LinearDamping = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wreckage Config")
	float AngularDamping = 1.0f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* WreckageMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UNiagaraComponent* ExplosionVfx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USoundCue* ExplosionSfx;

private:
	/** Replicated Fighter Movement */
	UPROPERTY(Replicated)
	FVector TargetLocation;

	UPROPERTY(Replicated)
	FQuat TargetRotation;

	void SynchroMovement(float DeltaTime);

public:	
	void SetPhysicalVelocity(const FVector& LinearVelocity, const FVector& AngularVelocityInRadians);
};
