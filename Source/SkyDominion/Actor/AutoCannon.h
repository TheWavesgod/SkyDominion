// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoCannon.generated.h"

class USoundCue;

USTRUCT(BlueprintType)
struct FCannonSounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Sound Config")
	USoundCue* SoundStart;

	UPROPERTY(EditAnywhere, Category = "Sound Config")
	USoundCue* SoundLoop;

	UPROPERTY(EditAnywhere, Category = "Sound Config")
	USoundCue* SoundEnd;

	UPROPERTY(EditAnywhere, Category = "Sound Config")
	float StartSoundDuration = 0.0f;
};

UCLASS()
class SKYDOMINION_API AAutoCannon : public AActor
{
	GENERATED_BODY()

public:	
	AAutoCannon();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	TSubclassOf<class AProjectile> BulletClass;

	// the amount of bullet shoot in a minute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	int FireRadte = 3000;

	// unit m/s
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	float InitSpeed = 1000.0f;

protected:
	virtual void BeginPlay() override;

	class AFighter* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	FCannonSounds CockpitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	FCannonSounds OuterSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	bool bDebugOuterSound = false;

	class UAudioComponent* StartSoundComponent;
	UAudioComponent* LoopSoundComponent;

	void PlayLoopSound();

	void SpawnBullet();

public:	
	UFUNCTION(BlueprintCallable)
	void FireStart();

	UFUNCTION(BlueprintCallable)
	void FireEnd();
};
