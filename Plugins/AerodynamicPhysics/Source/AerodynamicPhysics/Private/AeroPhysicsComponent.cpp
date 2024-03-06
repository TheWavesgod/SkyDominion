#include "AeroPhysicsComponent.h"
#include "Airplane.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AirplaneAnimInstance.h"
#include "Net/UnrealNetwork.h"

UAeroPhysicsComponent::UAeroPhysicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Airplane = GetOwner<AAirplane>();
	if (Airplane)
	{
		Mesh = Airplane->GetMesh();
	}
}

void UAeroPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeAnimationInstance();

	UWorld* World = GetWorld();
	if (World && World->GetNetMode() == ENetMode::NM_ListenServer)
	{
		if (Mesh)
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetCollisionObjectType(ECC_Pawn);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetEnableGravity(true);
			Mesh->SetMassOverrideInKg(FName("cog_jnt"), EmptyWeight);
			FVector WorldLocationOfMassCenter = Mesh->GetCenterOfMass();
			FVector LocalLocationOfMassCenter = Mesh->GetComponentTransform().InverseTransformPosition(WorldLocationOfMassCenter);
			Mesh->SetCenterOfMass(CenterOfMass - LocalLocationOfMassCenter, FName("cog_jnt"));
		}
	}

	InitializeArray();

	for (auto i : AerodynamicSurfaceSettings)
	{
		i.Config.OnValidate();
	}

	if (ThrusterSettings.Num() > 0)
	{
		AfterBurnerThresholdRatio = ThrusterSettings[0].MaxNormalThrust / ThrusterSettings[0].MaxExtraThrust;
	}
	else
	{
		AfterBurnerThresholdRatio = 0.0f;
	}
	/*FTimerHandle TimerHandle;
	float Interval = 1.0f / PhysicsDT;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::AeroPhysicsFun, Interval, true);*/
}

void UAeroPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DebugTick(DeltaTime);

	InitializeAnimationInstance();
	
	if (GetWorld() && GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		AeroPhysicsTick(DeltaTime);

		AddForceToMesh();
	}
}

void UAeroPhysicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAeroPhysicsComponent, WheelAnimVaribles);
	DOREPLIFETIME(UAeroPhysicsComponent, AerosufaceAnimVaribles);
	DOREPLIFETIME(UAeroPhysicsComponent, WheelRetreatedRatio); 
	DOREPLIFETIME(UAeroPhysicsComponent, bIsWheelsRetreated);
	DOREPLIFETIME(UAeroPhysicsComponent, CurrentThrusterRatio);
	DOREPLIFETIME(UAeroPhysicsComponent, RealThrusterRatio);
	DOREPLIFETIME(UAeroPhysicsComponent, GroundSpeed);
	DOREPLIFETIME(UAeroPhysicsComponent, GForce);
}

void UAeroPhysicsComponent::InitializeArray()
{
	// Wheel Array Init
	int32 TyreNum = Tyres.Num();
	WheelCalculationVariblesCache.Init(FWheelCacheVaribles(), TyreNum);
	WheelsForcesToAdd.Init(FVector::ZeroVector, TyreNum);
	WheelAnimVaribles.Init(FWheelAnimVaribles(), TyreNum);

	int32 ThrusterNum = ThrusterSettings.Num();
	ThrusterForcesToAdd.Init(FVector::ZeroVector, ThrusterNum);
	CurrentThrusters.Init(0.0f, ThrusterNum);

	int32 AeroSurfaceNum = AerodynamicSurfaceSettings.Num();
	AeroSufaceForcesAndTorques.Init(FBiVector(FVector::ZeroVector, FVector::ZeroVector), AeroSurfaceNum);
	AerosufaceAnimVaribles.Init(FAerosufaceAnimVaribles(), AeroSurfaceNum);
}

void UAeroPhysicsComponent::InitializeAnimationInstance()
{
	if (!Mesh || !Mesh->GetAnimInstance()) return;

	MeshAnimInstance = MeshAnimInstance == nullptr ? Cast<UAirplaneAnimInstance>(Mesh->GetAnimInstance()) : MeshAnimInstance;
	if (MeshAnimInstance)
	{
		if (MeshAnimInstance->GetAeroPhysicsComponent() == nullptr)
		{
			MeshAnimInstance->SetAeroPhysicsComponent(this);
		}
	}
}

void UAeroPhysicsComponent::AddForceToMesh()
{
	if (!Mesh) return;

	for (int i = 0; i < WheelsForcesToAdd.Num(); ++i)
	{
		Mesh->AddForceAtLocation(
			WheelsForcesToAdd[i] * 100.0f,
			WheelCalculationVariblesCache[i].ImpactLocation
		);
	}
	for (int i = 0; i < ThrusterForcesToAdd.Num(); ++i)
	{
		Mesh->AddForceAtLocation(
			ThrusterForcesToAdd[i] * 100.0f,
			Airplane->GetTransform().TransformPosition(ThrusterSettings[i].EngineLocation)
		);
	}
	/*for (int i = 0; i < AeroSufaceForcesAndTorques.Num(); ++i)
	{
		Mesh->AddForce(Airplane->GetTransform().TransformVector(AeroSufaceForcesAndTorques[i].f) * 100.0f);
		Mesh->AddTorqueInRadians(Airplane->GetTransform().TransformVector(AeroSufaceForcesAndTorques[i].t) * 10000.0f);
	}*/
	Mesh->AddForce(Airplane->GetTransform().TransformVector(AeroSufaceTotalForceAndTorque.f) * 100.0f);
	Mesh->AddTorqueInRadians(Airplane->GetTransform().TransformVector(AeroSufaceTotalForceAndTorque.t) * 10000.0f);
	//DrawDebugLine(GetWorld(), Airplane->GetTransform().GetLocation(), Airplane->GetTransform().GetLocation() + Airplane->GetTransform().TransformVector(AeroSufaceTotalForceAndTorque.t), FColor::Red, false, 0.0f);
}

void UAeroPhysicsComponent::AeroPhysicsTick(float DeltaTime)
{
	if (!Mesh) return;

	AeroParametersCalculation(DeltaTime);

	WheelsForceCalculation(DeltaTime);

	ThrusterForceCalculation(DeltaTime);

	CalculateFlyControl(DeltaTime);

	AerodynamicFroceCalculation(DeltaTime);
}

void UAeroPhysicsComponent::AeroPhysicsFun()
{
}

void UAeroPhysicsComponent::AeroParametersCalculation(float DeltaTime)
{
	//if (!Mesh) return;

	LastFrameMeshVelocity = MeshVelocity;
	MeshVelocity = Mesh->GetPhysicsLinearVelocity();
	MeshAcceleration = (MeshVelocity - LastFrameMeshVelocity) / DeltaTime;

	MeshAngularVelocityInRadians = Mesh->GetPhysicsAngularVelocityInRadians();

	//GForce = -MeshVelocity.Dot(Mesh->GetForwardVector()) * MeshAngularVelocityInRadians.Dot(Mesh->GetRightVector()) * 0.01f / 9.81f;
	GForce = CalculateCurrentGForce(DeltaTime);

	FVector ForwardVelocity = MeshVelocity.ProjectOnTo(Mesh->GetForwardVector());
	GroundSpeed = ForwardVelocity.Size() * 0.036;

	FVector RightPlaneVelocity = FVector::VectorPlaneProject(MeshVelocity, Mesh->GetRightVector());
	if (RightPlaneVelocity.Size() > 100.0f)
	{
		FVector RightPlaneVelocityDir = RightPlaneVelocity;
		RightPlaneVelocityDir.Normalize();
		float SignValue = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightPlaneVelocity, Mesh->GetUpVector()))) < 90.0f ? -1.0f : 1.0f;
		AngleOfAttack = SignValue * FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightPlaneVelocityDir, Mesh->GetForwardVector())));
	}
	else
	{
		AngleOfAttack = 0.0f;
	}
}

float UAeroPhysicsComponent::CalculateCurrentGForce(float DeltaTime)
{
	return MeshAcceleration.Dot(Mesh->GetUpVector()) / 981.0f;
}

void UAeroPhysicsComponent::WheelsForceCalculation(float DeltaTime)
{
	if (bIsWheelsRetreated)
	{
		WheelRetreatedRatio = FMath::FInterpConstantTo(WheelRetreatedRatio, 1.0f, DeltaTime, 0.3f);
	}
	else
	{
		WheelRetreatedRatio = FMath::FInterpConstantTo(WheelRetreatedRatio, 0.0f, DeltaTime, 0.3f);
	}

	if (WheelRetreatedRatio >= 1.0f) { return; }

	WheelsRaycastAndVariablesCache();

	for (int i = 0; i < WheelCalculationVariblesCache.Num(); ++i)
	{
		if (!WheelCalculationVariblesCache[i].bIsHit)
		{
			WheelsForcesToAdd[i] = FVector::ZeroVector;
			WheelAnimVaribles[i].SuspensionDisplacement = FMath::FInterpTo(WheelAnimVaribles[i].SuspensionDisplacement, 0.0f, DeltaTime, 5.0f);
			WheelAnimVaribles[i].WheelRotation.Pitch = FMath::FInterpTo(WheelAnimVaribles[i].WheelRotation.Pitch, 0.0f, DeltaTime, 5.0f);
			continue;
		}

		/** Suspension Force Calculation */
		float SpringWholeLength = Tyres[i].SuspensionSettings.SuspensionMaxRaise + Tyres[i].SuspensionSettings.SuspensionMaxDrop + Tyres[i].SuspensionSettings.SpringPreLoadLength;
		float SpringCompressLength = WheelCalculationVariblesCache[i].Distance - Tyres[i].WheelSettings.WheelRadius - WheelRayOffset;
		float SpringForceLength = SpringWholeLength - SpringCompressLength;
		float SpringForce = SpringForceLength * Tyres[i].SuspensionSettings.SpringRate * 0.01f;

		float SuspensionDistanceDisplacementVel = (WheelCalculationVariblesCache[i].LastFrameDistance - WheelCalculationVariblesCache[i].Distance) / DeltaTime;
		float SuspensionDampingAspect = FMath::Abs(SuspensionDistanceDisplacementVel) > SuspensionStaticThreshold ? SuspensionDistanceDisplacementVel : 0.0f;
		float SuspensionDampingForce = Tyres[i].SuspensionSettings.SuspensionDampingRatio * SuspensionDampingAspect;
		//SuspensionDampingForce = FMath::Abs(SuspensionDampingForce) > SpringForce ? 0.0f : SuspensionDampingForce;

		/*if (FMath::Abs(SuspensionDistanceDisplacementVel) > SuspensionStaticThreshold)
		{
			AddDebugMessageOnScreen(0.0f, FColor::Blue, FString::Printf(TEXT("Suspension %d: Moving"), i));
		}
		else
		{
			AddDebugMessageOnScreen(0.0f, FColor::Blue, FString::Printf(TEXT("Suspension %d: Static"), i));
		}*/

		float ActualSuspensionForce = SpringForce + SuspensionDampingForce;

		FVector SuspensionWorldAxis = Airplane->GetActorTransform().TransformVector(Tyres[i].SuspensionSettings.SuspensionAxis);
		FVector SuspensionForce = ActualSuspensionForce * (-SuspensionWorldAxis);

		WheelAnimVaribles[i].SuspensionDisplacement = SpringForceLength - Tyres[i].SuspensionSettings.SpringPreLoadLength - Tyres[i].SuspensionSettings.SuspensionMaxDrop;


		/** Friction Calculation */
		FVector WheelVelocity = (WheelCalculationVariblesCache[i].ImpactLocation - WheelCalculationVariblesCache[i].LastFrameImpactLocation) / DeltaTime;
		FVector WheelPlaneVelocity = FVector::VectorPlaneProject(WheelVelocity, WheelCalculationVariblesCache[i].Normal);

		// Calculate Friction Ratio
		float SurfaceFrictionRatio = WheelCalculationVariblesCache[i].SurfaceMatrial ? WheelCalculationVariblesCache[i].SurfaceMatrial->Friction : 0.7;
		float WheelFrictionRatio = Tyres[i].WheelSettings.WheelFrictionRatio;
		// Friction Ratio Combine mode = Average
		float FrictionRatio = (SurfaceFrictionRatio + WheelFrictionRatio) / 2.0f;
		float StaticFrictionRatio = FrictionRatio + 0.1;

		// Suspension Force Resolution
		FVector NormalSuspensionForce = SuspensionForce.ProjectOnToNormal(WheelCalculationVariblesCache[i].Normal);
		FVector PlaneSuspensionForce = FVector::VectorPlaneProject(SuspensionForce, WheelCalculationVariblesCache[i].Normal);

		// Calculate the wheels' forward and right direction
		FVector WheelForwardDir = Mesh->GetForwardVector();
		FVector WheelRightDir = Mesh->GetRightVector();
		if (Tyres[i].WheelSettings.bAffectedBySteering)
		{
			WheelCalculationVariblesCache[i].CurrentWheelTurnRate = FMath::FInterpTo(WheelCalculationVariblesCache[i].CurrentWheelTurnRate, TargetWheelTurnRate, DeltaTime, 5.0f);
			float WheelSteeringDegree = WheelCalculationVariblesCache[i].CurrentWheelTurnRate * Tyres[i].WheelSettings.MaxSteerAngle;
			WheelAnimVaribles[i].WheelRotation.Yaw = WheelSteeringDegree;

			WheelForwardDir = WheelForwardDir.RotateAngleAxis(WheelSteeringDegree, -SuspensionWorldAxis);
			WheelRightDir = WheelRightDir.RotateAngleAxis(WheelSteeringDegree, -SuspensionWorldAxis);
		}
		WheelForwardDir = FVector::VectorPlaneProject(WheelForwardDir, WheelCalculationVariblesCache[i].Normal);
		WheelForwardDir = WheelForwardDir.Normalize() ? WheelForwardDir : FVector::ZeroVector;
		WheelRightDir = FVector::VectorPlaneProject(WheelRightDir, WheelCalculationVariblesCache[i].Normal);
		WheelRightDir = WheelRightDir.Normalize() ? WheelRightDir : FVector::ZeroVector;

		FVector WheelRightVelocity = WheelPlaneVelocity.ProjectOnTo(WheelRightDir);
		FVector WheelForwardVelocity = WheelPlaneVelocity.ProjectOnTo(WheelForwardDir);

		FVector WheelFrictionForce;
		FVector RightSuspensionForce = PlaneSuspensionForce.ProjectOnTo(WheelRightDir);
		FVector ForwardSuspensionForce = PlaneSuspensionForce.ProjectOnTo(WheelForwardDir);

		if (WheelPlaneVelocity.Size() > WheelStaticThreshold)
		{
			float MaxStaticFriction = NormalSuspensionForce.Size() * StaticFrictionRatio * 100.0f;

			float TargetRightFriction = WheelRightVelocity.Size() * NormalSuspensionForce.Size() / (9.8f * WheelTurnFrictionRatio);
			FVector TargetRightFrictionForce = -WheelRightVelocity / WheelRightVelocity.Size() * TargetRightFriction;
			float WheelDrag = NormalSuspensionForce.Size() * Tyres[i].WheelSettings.WheelDragRatio;
			FVector WheelDragForce = -WheelForwardVelocity / WheelForwardVelocity.Size() * WheelDrag;
			FVector TargetWheelFricion = TargetRightFrictionForce + WheelDragForce + PlaneSuspensionForce;

			/*AddDebugMessageOnScreen(DeltaTime, FColor::Red, FString::Printf(TEXT("Wheel %d TargetRightFriction: %f"), i, TargetRightFriction));*/

			if (TargetWheelFricion.Size() > MaxStaticFriction)
			{
				WheelFrictionForce = WheelPlaneVelocity / WheelPlaneVelocity.Size() * NormalSuspensionForce.Size() * FrictionRatio;
				/*AddDebugMessageOnScreen(DeltaTime, FColor::Cyan, FString::Printf(TEXT("Wheel %d: sliding"), i));*/
			}
			else
			{
				float BrakeForce = 0.0;
				if (Tyres[i].WheelSettings.bAffectedByBrake && BrakeForceRatio > 0.0f)
				{
					float MaxBrakeForce = MaxStaticFriction - TargetWheelFricion.Size();
					BrakeForce = MaxBrakeForce * BrakeForceRatio;
				}
				float TargetForwardFriction = WheelForwardVelocity.Size() * NormalSuspensionForce.Size() / (9.8f * WheelTurnFrictionRatio);
				float ActualForwardFriction = TargetForwardFriction < (BrakeForce + WheelDrag) ? TargetForwardFriction : (BrakeForce + WheelDrag);
				FVector ForwardFrictionForce = -WheelForwardVelocity / WheelForwardVelocity.Size() * ActualForwardFriction;

				WheelFrictionForce = ForwardFrictionForce + TargetRightFrictionForce + PlaneSuspensionForce ;

				/*AddDebugMessageOnScreen(DeltaTime, FColor::Cyan, FString::Printf(TEXT("Wheel %d: static friction"), i));
				AddDebugMessageOnScreen(DeltaTime, FColor::Red, FString::Printf(TEXT("Wheel %d TargetForwardFriction: %f"), i, TargetForwardFriction));*/
			}
		}
		else
		{
			if (PlaneSuspensionForce.Size() > NormalSuspensionForce.Size() * StaticFrictionRatio)
			{
				WheelFrictionForce = (PlaneSuspensionForce / PlaneSuspensionForce.Size()) * NormalSuspensionForce.Size() * FrictionRatio;
			}
			else
			{
				WheelFrictionForce = PlaneSuspensionForce;
			}
		}

		WheelsForcesToAdd[i] = NormalSuspensionForce + WheelFrictionForce;

		float WheelPitchDegree = WheelForwardVelocity.Size() * DeltaTime / Tyres[i].WheelSettings.WheelRadius / PI * 180.0f; 
		float WheelPitchDir = WheelForwardVelocity.Dot(WheelForwardDir) > 0.0f ? 1.0f : -1.0f;
		WheelAnimVaribles[i].WheelRotation.Pitch -= WheelPitchDegree * WheelPitchDir;
		WheelAnimVaribles[i].WheelRotation.Pitch = WheelAnimVaribles[i].WheelRotation.Pitch > 360.0f ? WheelAnimVaribles[i].WheelRotation.Pitch - 360.0f : WheelAnimVaribles[i].WheelRotation.Pitch;
		WheelAnimVaribles[i].WheelRotation.Pitch = WheelAnimVaribles[i].WheelRotation.Pitch < 0.0f ? WheelAnimVaribles[i].WheelRotation.Pitch + 360.0f : WheelAnimVaribles[i].WheelRotation.Pitch;

		/*DrawDebugLine(GetWorld(), WheelCalculationVariblesCache[i].ImpactLocation, WheelCalculationVariblesCache[i].ImpactLocation + SuspensionForce * 10.0f, FColor::Green, false, 0.0f);
		DrawDebugLine(GetWorld(), WheelCalculationVariblesCache[i].ImpactLocation, WheelCalculationVariblesCache[i].ImpactLocation + WheelFrictionForce * 10.0f, FColor::Red, false, 0.0f);*/
	}
}

void UAeroPhysicsComponent::WheelsRaycastAndVariablesCache()
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (int i = 0; i < Tyres.Num(); ++i)
		{
			FVector CurrentWheelLocation = Mesh->GetSocketLocation(Tyres[i].WheelBoneName);
			float WheelRadius = Tyres[i].WheelSettings.WheelRadius;
			FVector ChassisAxis = Airplane->GetActorTransform().TransformVector(Tyres[i].SuspensionSettings.SuspensionAxis);
			float ChassisMaxRaise = Tyres[i].SuspensionSettings.SuspensionMaxRaise;
			float ChassisMaxDrop = Tyres[i].SuspensionSettings.SuspensionMaxDrop;

			FVector Start = CurrentWheelLocation - ChassisAxis * (ChassisMaxRaise + WheelRayOffset);
			FVector End = CurrentWheelLocation + ChassisAxis * (ChassisMaxDrop + WheelRadius);
			FHitResult WheelHitResult;
			FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
			QueryParams.AddIgnoredActor(Airplane);
			QueryParams.AddIgnoredComponent(Mesh);
			QueryParams.bReturnPhysicalMaterial = true;

			World->LineTraceSingleByChannel(
				WheelHitResult,
				Start,
				End,
				ECollisionChannel::ECC_Pawn,
				QueryParams
			);

			WheelCalculationVariblesCache[i].bIsHit = WheelHitResult.bBlockingHit;
			WheelCalculationVariblesCache[i].LastFrameImpactLocation = WheelCalculationVariblesCache[i].ImpactLocation;
			WheelCalculationVariblesCache[i].ImpactLocation = WheelHitResult.bBlockingHit ? WheelHitResult.ImpactPoint : FVector::ZeroVector;
			WheelCalculationVariblesCache[i].LastFrameDistance = WheelCalculationVariblesCache[i].Distance;
			WheelCalculationVariblesCache[i].Distance = WheelHitResult.bBlockingHit ? (WheelHitResult.TraceStart - WheelHitResult.Location).Size() : (Start - End).Size();
			WheelCalculationVariblesCache[i].Normal = WheelHitResult.ImpactNormal;
			WheelCalculationVariblesCache[i].SurfaceMatrial = WheelHitResult.PhysMaterial.Get();
		}
	}
}

void UAeroPhysicsComponent::ThrusterForceCalculation(float DeltaTime)
{
	RealThrusterRatio = FMath::FInterpTo(RealThrusterRatio, CurrentThrusterRatio, DeltaTime, 1.0f);

	for (int i = 0; i < ThrusterSettings.Num(); ++i)
	{
		float TargetThruster = RealThrusterRatio * ThrusterSettings[i].MaxExtraThrust * 9.8f;

		// Consider the effect of air density
		TargetThruster *= 1.0f;

		CurrentThrusters[i] = TargetThruster;

		FVector ThrusterForcesAtLocal = ThrusterSettings[i].ThrustAixs.GetSafeNormal() * CurrentThrusters[i];

		//ThrusterForcesToAdd[i] = ThrusterForcesAtLocal;
		ThrusterForcesToAdd[i] = Airplane->GetTransform().TransformVector(ThrusterForcesAtLocal);
	}
}

void UAeroPhysicsComponent::AerodynamicFroceCalculation(float DeltaTime)
{
	AeroSufaceTotalForceAndTorque.f = FVector::ZeroVector;
	AeroSufaceTotalForceAndTorque.t = FVector::ZeroVector;
	for (int i = 0; i < AerodynamicSurfaceSettings.Num(); ++i)
	{
		if (bShowAeroSufaceDubugBox)
		{
			DrawDebugSolidBox(GetWorld(),
				Mesh->GetComponentTransform().TransformPosition(AerodynamicSurfaceSettings[i].RelativePosition),
				FVector(AerodynamicSurfaceSettings[i].Config.Chord * 50.0f, AerodynamicSurfaceSettings[i].Config.Span * 50.0f, 5.0f),
				Mesh->GetComponentTransform().TransformRotation(AerodynamicSurfaceSettings[i].GetCurrentRelativeRotation()),
				FColor::Purple,
				false,
				DeltaTime
			);
		}

		float RotDegree = 0.0f;
		if (AerodynamicSurfaceSettings[i].ControlConfig.bPitchControl)
		{
			RotDegree += CalculateRotDegree(PitchControl, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y);
		}
		if (AerodynamicSurfaceSettings[i].ControlConfig.bRollControl)
		{
			RotDegree += CalculateRotDegree(RollControl, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y);
		}
		if (AerodynamicSurfaceSettings[i].ControlConfig.bYawControl)
		{
			RotDegree += CalculateRotDegree(YawControl, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y);
			//AddDebugMessageOnScreen(DeltaTime, FColor::Cyan, FString::Printf(TEXT("RotDegree: %f"), RotDegree));
		}
		if (AerodynamicSurfaceSettings[i].ControlConfig.bFlapControl)
		{
			RotDegree += CalculateRotDegree(FlapControl, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y);
		}
		RotDegree = AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X < AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y ?
			FMath::Clamp(RotDegree, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y) :
				FMath::Clamp(RotDegree, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.Y, AerodynamicSurfaceSettings[i].ControlConfig.ControlAngleMapDegree.X);

			if (AerodynamicSurfaceSettings[i].ControlConfig.bIsFullMove)
			{
				AerodynamicSurfaceSettings[i].SetFullMoveAngle(RotDegree);
			}
			else
			{
				AerodynamicSurfaceSettings[i].SetFlapAngle(RotDegree);
			}
			AerosufaceAnimVaribles[i].RotDegree = RotDegree;

			FBiVector AerodynamicForces = AerodynamicSurfaceSettings[i].CalculateAerodynamicForces(
				Mesh->GetComponentTransform().InverseTransformVector(-Mesh->GetPhysicsLinearVelocity()),
				1.225f
			);

			AeroSufaceTotalForceAndTorque.f += AerodynamicForces.f;
			AeroSufaceTotalForceAndTorque.t += AerodynamicForces.t;
			AeroSufaceForcesAndTorques[i] = AerodynamicForces;

			
			if (false)
			{
				DrawDebugLine(
					GetWorld(),
					Mesh->GetComponentTransform().TransformPosition(AerodynamicSurfaceSettings[i].RelativePosition),
					Mesh->GetComponentTransform().TransformPosition(AerodynamicSurfaceSettings[i].RelativePosition) + Mesh->GetComponentTransform().TransformVector(AeroSufaceForcesAndTorques[i].f) * 500.0f,
					FColor::Yellow,
					false,
					DeltaTime
				);
				DrawDebugLine(
					GetWorld(),
					Mesh->GetComponentTransform().TransformPosition(AerodynamicSurfaceSettings[i].RelativePosition),
					Mesh->GetComponentTransform().TransformPosition(AerodynamicSurfaceSettings[i].RelativePosition) + Mesh->GetComponentTransform().TransformVector(AeroSufaceForcesAndTorques[i].t) * 500.0f,
					FColor::Green,
					false,
					DeltaTime
				);
			}
	}
	

}

float UAeroPhysicsComponent::CalculateRotDegree(float ControlAxis, float X, float Y)
{
	if (ControlAxis < 0)
	{
		return FMath::Abs(ControlAxis) * X;
	}
	else
	{
		return FMath::Abs(ControlAxis) * Y;
	}
}

void UAeroPhysicsComponent::CalculateFlyControl(float DeltaTime)
{
	float SpeedIndex = FMath::Square(GroundSpeed / 100.0f);

	float GSpeedIndex = SpeedIndex * FMath::Abs(GroundSpeed / 100.0f);
	float TargetPitchControlLimitRatio = FMath::Clamp(PitchControlLimitRatio / GSpeedIndex, 0.0f, 1.0f);
	if (FMath::Abs(GroundSpeed) > 50.0f && FMath::Abs(PitchInput) < 0.005)
	{
		TargetPitchControl = PitchInput * TargetPitchControlLimitRatio;
		if (PitchInput > 0.0f)
		{
			TargetPitchControl *= 0.25f;
		}
		PitchControl = FMath::FInterpTo(PitchControl, TargetPitchControl, DeltaTime, 5.0f);
	}
	else
	{
		TargetPitchControl = PitchInput * TargetPitchControlLimitRatio;
		if (PitchInput > 0.0f)
		{
			TargetPitchControl *= 0.25f;
		}
		PitchControl = FMath::FInterpTo(PitchControl, TargetPitchControl, DeltaTime, 5.0f);
	}

	float TargetRollControlLimitRatio = FMath::Clamp(RollControlLimitRatio / SpeedIndex, 0.0f, 1.0f);
	if (FMath::Abs(GroundSpeed) > 50.0f && FMath::Abs(RollInput) < 0.005)
	{
		float AirplaneRollSpeed = FMath::RadiansToDegrees(MeshAngularVelocityInRadians.Dot(Mesh->GetForwardVector()));
		if (FMath::Abs(AirplaneRollSpeed) > 1.0f)
		{
			TargetRollControl = FMath::Clamp(AirplaneRollSpeed * 0.01, -1.0f, 1.0f) * TargetRollControlLimitRatio;
		}
		else
		{
			TargetRollControl = RollInput;
		}

		RollControl = FMath::FInterpTo(RollControl, TargetRollControl, DeltaTime, 3.0f);
	}
	else
	{
		TargetRollControl = RollInput * TargetRollControlLimitRatio;
		RollControl = FMath::FInterpTo(RollControl, TargetRollControl, DeltaTime, 5.0f);
	}

	YawControl = FMath::FInterpTo(YawControl, YawInput, DeltaTime, 5.0f);

	FlapControl = FMath::FInterpTo(FlapControl, FlapInput, DeltaTime, 5.0f);
}

void UAeroPhysicsComponent::InterpAeroControl(float DeltaTime)
{
	PitchControl = FMath::FInterpConstantTo(PitchControl, PitchInput, DeltaTime, 5.0f);
	RollControl = FMath::FInterpConstantTo(RollControl, RollInput, DeltaTime, 5.0f);
	YawControl = FMath::FInterpConstantTo(YawControl, YawInput, DeltaTime, 5.0f);
	FlapControl = FMath::FInterpConstantTo(FlapControl, FlapInput, DeltaTime, 5.0f);
}



void UAeroPhysicsComponent::SetWheelsBrake(float AxisValue)
{
	BrakeForceRatio = FMath::Clamp(AxisValue, 0.0f, 1.0f);
}

void UAeroPhysicsComponent::SetSteeringWheels(float AxisValue)
{
	TargetWheelTurnRate = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void UAeroPhysicsComponent::SetWheelsRetreated(bool bIsRetreated)
{
	bIsWheelsRetreated = bIsRetreated;
}

void UAeroPhysicsComponent::SetAddThruster(float AxisValue)
{
	float Axis = FMath::Clamp(AxisValue, -1.0f, 1.0f);

	float TargetCurrentThrusterRatio = CurrentThrusterRatio + Axis * ThrusterRatioAddPerSecond * GetWorld()->GetDeltaSeconds();

	CurrentThrusterRatio = FMath::Clamp(TargetCurrentThrusterRatio, 0.0f, 1.0f);
}

void UAeroPhysicsComponent::SetAeroPitchControl(float AxisValue)
{
	PitchInput = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void UAeroPhysicsComponent::SetAeroRollControl(float AxisValue)
{
	RollInput = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void UAeroPhysicsComponent::SetAeroYawControl(float AxisValue)
{
	YawInput = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void UAeroPhysicsComponent::SetAeroFlapControl(float AxisValue)
{
	FlapInput = FMath::Clamp(AxisValue, -1.0f, 1.0f);
}

void UAeroPhysicsComponent::DebugTick(float DeltaTime)
{
	//AddDebugMessageOnScreen(0.0f, FColor::Blue, FString::Printf(TEXT("Jet's Weight: %d kg"), FMath::RoundToInt32(Mesh->GetMass())));
	/*for (auto i : WheelsForcesToAdd)
	{
		AddDebugMessageOnScreen(0.0f, FColor::Green, FString::Printf(TEXT("WheelForce: %d"), FMath::RoundToInt32(i.Length())));
	}*/
	/*if (Mesh)
	{
		DrawDebugSphere(GetWorld(), Mesh->GetCenterOfMass(), 30.0f, 8, FColor::Red, false, 0.0f);
	}*/
	/*for (int i = 0; i < CurrentThrusters.Num(); ++i)
	{
		AddDebugMessageOnScreen(0.0f, FColor::Red, FString::Printf(TEXT("Thruster %d: %d"), i, FMath::RoundToInt32(CurrentThrusters[i])));
	}*/
	//AddDebugMessageOnScreen(0.0f, FColor::Red, FString::Printf(TEXT("GroundSpeed: %d, GForce: %f"), FMath::CeilToInt(GroundSpeed), GForce));
}

void UAeroPhysicsComponent::AddDebugMessageOnScreen(const float DisplayTime, const FColor Color, const FString DiplayString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			DisplayTime,
			Color,
			DiplayString
		);
	}
}

