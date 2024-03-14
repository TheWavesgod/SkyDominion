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

public:	
	UFUNCTION(BlueprintCallable)
	void FireStart();

	UFUNCTION(BlueprintCallable)
	void FireEnd();
};
