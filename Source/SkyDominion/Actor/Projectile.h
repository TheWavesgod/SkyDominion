// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SKYDOMINION_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UCapsuleComponent* CollisionBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* VisualMesh;

public:	
	virtual void Tick(float DeltaTime) override;

};
