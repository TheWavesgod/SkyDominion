// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DisplayAirplane.generated.h"

UCLASS()
class SKYDOMINION_API ADisplayAirplane : public AActor
{
	GENERATED_BODY()
	
public:	
	ADisplayAirplane();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* Mesh;

public:
	void SetDisplayPlaneByIndex(const int Index);
};
