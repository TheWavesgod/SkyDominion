#include "FighterWreckage.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

AFighterWreckage::AFighterWreckage()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	WreckageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WreckageMesh")); 
	SetRootComponent(WreckageMesh);

	ExplosionVfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionVfx")); 
	ExplosionVfx->SetupAttachment(RootComponent);
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
		SetLifeSpan(30.0f);
	}
	else
	{
		WreckageMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
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
	if (HasAuthority())
	{
		WreckageMesh->SetPhysicsLinearVelocity(LinearVelocity);
		WreckageMesh->SetPhysicsAngularVelocityInRadians(AngularVelocityInRadians);
	}
}


