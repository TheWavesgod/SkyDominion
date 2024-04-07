#include "FighterWreckage.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AFighterWreckage::AFighterWreckage()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	WreckageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WreckageMesh")); 
	SetRootComponent(WreckageMesh);

	ExplosionVfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionVfx")); 
	ExplosionVfx->SetupAttachment(RootComponent);
	ExplosionVfx->bAutoActivate = false;
}

void AFighterWreckage::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		WreckageMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WreckageMesh->SetCollisionObjectType(ECC_PhysicsBody);
		WreckageMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WreckageMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
		WreckageMesh->SetSimulatePhysics(true);
		WreckageMesh->SetEnableGravity(true);
		WreckageMesh->SetMassOverrideInKg(NAME_None, Weight);
		WreckageMesh->SetLinearDamping(LinearDamping);
		WreckageMesh->SetAngularDamping(AngularDamping);
		SetLifeSpan(30.0f);
	}
	else
	{
		WreckageMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	ExplosionVfx->Activate(true);
	if(ExplosionSfx)
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSfx, GetActorLocation(), 1.8f, 1.0f, 0.0f, SoundAttenuation);
}

void AFighterWreckage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFighterWreckage, TargetLocation);
	DOREPLIFETIME(AFighterWreckage, TargetRotation);
}

void AFighterWreckage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SynchroMovement(DeltaTime);
}

void AFighterWreckage::SynchroMovement(float DeltaTime)
{
	if (HasAuthority())
	{
		TargetLocation = GetActorLocation();
		TargetRotation = GetActorQuat();
	}
	else
	{
		float LocationInterpSpeed = 30.0f;
		float RotationInterpSpeed = 30.0f;
		FVector SmoothLocation = FMath::Lerp(GetActorLocation(), TargetLocation, DeltaTime * LocationInterpSpeed);
		FQuat SmoothRotation = FMath::Lerp(GetActorQuat(), TargetRotation, DeltaTime * RotationInterpSpeed);
		SetActorLocationAndRotation(TargetLocation, TargetRotation);
	}
}

void AFighterWreckage::SetPhysicalVelocity(const FVector& LinearVelocity, const FVector& AngularVelocityInRadians)
{
	if (!this) return;

	if (HasAuthority())
	{
		WreckageMesh->SetPhysicsLinearVelocity(LinearVelocity);
		WreckageMesh->SetPhysicsAngularVelocityInRadians(AngularVelocityInRadians);
	}
}


