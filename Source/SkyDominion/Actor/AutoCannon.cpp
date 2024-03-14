// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoCannon.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"

AAutoCannon::AAutoCannon()
{
	PrimaryActorTick.bCanEverTick = true;

	StartSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StartSoundComponent"));
	StartSoundComponent->bAutoActivate = false;
	StartSoundComponent->bStopWhenOwnerDestroyed = true;

	LoopSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LoopAudioComponent"));
	LoopSoundComponent->bAutoActivate = false; 
	LoopSoundComponent->bStopWhenOwnerDestroyed = true;
}

void AAutoCannon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAutoCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 10000.0f, FColor::Red, false, 0.0f);
}

void AAutoCannon::FireStart()
{
	Owner = Owner == nullptr ? Cast<AFighter>(this->GetOwner()) : Owner;

	if (Owner)
	{
		FCannonSounds SoundsToUse;
		if (Owner->IsLocallyControlled() && !bDebugOuterSound)
		{
			SoundsToUse = CockpitSound;
		}
		else
		{
			SoundsToUse = OuterSound;
		}

		if (SoundsToUse.SoundStart && LoopSoundComponent && !LoopSoundComponent->IsPlaying())
		{
			StartSoundComponent->SetSound(SoundsToUse.SoundStart);
			StartSoundComponent->Play();
		}

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::PlayLoopSound);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, SoundsToUse.StartSoundDuration, false);
	}
}

void AAutoCannon::FireEnd()
{
	Owner = Owner == nullptr ? Cast<AFighter>(this->GetOwner()) : Owner;

	if (Owner)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

		if (StartSoundComponent && StartSoundComponent->IsPlaying())
		{
			StartSoundComponent->Stop();
		}

		if (LoopSoundComponent && LoopSoundComponent->IsPlaying())
		{
			LoopSoundComponent->Stop();
		}

		USoundCue* EndSound;
		if (Owner->IsLocallyControlled() && !bDebugOuterSound)
		{
			EndSound = CockpitSound.SoundEnd;
		}
		else
		{
			EndSound = OuterSound.SoundEnd;
		}
		if (EndSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EndSound, GetActorLocation());
		}
	}
}

void AAutoCannon::PlayLoopSound()
{
	if (Owner)
	{
		USoundCue* LoopSound;
		if (Owner->IsLocallyControlled() && !bDebugOuterSound)
		{
			LoopSound = CockpitSound.SoundLoop;
		}
		else
		{
			LoopSound = OuterSound.SoundLoop;
		}

		if (LoopSound && LoopSoundComponent && !LoopSoundComponent->IsPlaying())
		{
			LoopSoundComponent->SetSound(LoopSound);
			LoopSoundComponent->Play();
		}
	}
}

void AAutoCannon::SpawnBullet()
{
	if (BulletClass)
	{

	}
}
