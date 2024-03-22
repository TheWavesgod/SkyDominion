// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoCannon.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundAttenuation.h"
#include "NiagaraComponent.h"
#include "Components/SceneComponent.h"

AAutoCannon::AAutoCannon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StartSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StartSoundComponent"));
	StartSoundComponent->bAutoActivate = false;
	StartSoundComponent->bStopWhenOwnerDestroyed = true;
	StartSoundComponent->AttenuationSettings = AutoCannonSoundAttenuation;

	LoopSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LoopAudioComponent"));
	LoopSoundComponent->bAutoActivate = false; 
	LoopSoundComponent->bStopWhenOwnerDestroyed = true;
	LoopSoundComponent->AttenuationSettings = AutoCannonSoundAttenuation;

	CurrentBulletLeft = MaxCarring;

	FiringFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FiringFX"));
	FiringFX->SetupAttachment(RootComponent);
	FiringFX->bAutoActivate = false;
}

void AAutoCannon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAutoCannon::Destroyed()
{
	/*GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetWorld()->GetTimerManager().ClearTimer(FiringTimer);*/

	Super::Destroyed();
}

void AAutoCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 10000.0f, FColor::Red, false, 0.0f);
}

void AAutoCannon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutoCannon, CurrentBulletLeft);
}

void AAutoCannon::FireStart()
{
	if (CurrentBulletLeft == 0) return;

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

		GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &ThisClass::SpawnBullet, 60.0f / FireRadte, true);

		if (FiringFX)
		{
			FiringFX->SetActive(true);
		}
	}
}

void AAutoCannon::FireEnd()
{
	Owner = Owner == nullptr ? Cast<AFighter>(this->GetOwner()) : Owner;

	if (Owner)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

		bool bNeedEndSound = false;
		if (StartSoundComponent && StartSoundComponent->IsPlaying())
		{
			StartSoundComponent->Stop();
			bNeedEndSound = true;
		}

		if (LoopSoundComponent && LoopSoundComponent->IsPlaying())
		{
			LoopSoundComponent->Stop();
			bNeedEndSound = true;
		}

		if (bNeedEndSound)
		{
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

		GetWorld()->GetTimerManager().ClearTimer(FiringTimer);

		if (FiringFX)
		{
			FiringFX->SetActive(false);
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
	if (CurrentBulletLeft == 0)
	{
		FireEnd();
	}

	if (!HasAuthority()) return;
	
	if (BulletClass)
	{
		FActorSpawnParameters BulletSpawnParameter;
		APawn* bulletInstigator = this->GetOwner<APawn>();
		BulletSpawnParameter.Owner = GetOwner();
		BulletSpawnParameter.Instigator = bulletInstigator;
		AProjectile* bullet = GetWorld()->SpawnActor<AProjectile>(BulletClass, GetActorTransform(), BulletSpawnParameter);
		if (bullet)
		{
			if (TracerBulletCounter <= 0)
			{
				bullet->SetVisualMeshVisibility(true);
				TracerBulletCounter = TracerBulletGap;
			}
			else
			{
				bullet->SetVisualMeshVisibility(false);
				--TracerBulletCounter;
			}
			//bullet->GetProjectileMovementComponent()->Velocity = GetActorForwardVector() * InitSpeed * 100.0f;
			CurrentBulletLeft = FMath::Clamp(CurrentBulletLeft - 1, 0, MaxCarring);
		}
	}
}
