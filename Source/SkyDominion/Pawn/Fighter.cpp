// Fill out your copyright notice in the Description page of Project Settings.


#include "Fighter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AerodynamicPhysics/public/AeroPhysicsComponent.h"
#include "F35SoundSystem/Sounds_F35.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "SkyDominion/Actor/RadarComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "SkyDominion/Actor/AutoCannon.h"

AFighter::AFighter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	MainCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainCameraSpringArm"));
	MainCameraSpringArm->SetupAttachment(RootComponent);

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(MainCameraSpringArm);

	ThrusterFXLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterLeftFX"));
	ThrusterFXRight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterRightFX"));
	ThrusterFXLeft->SetupAttachment(RootComponent);
	ThrusterFXRight->SetupAttachment(RootComponent);

	MarkWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MarkWidget"));
	MarkWidget->SetupAttachment(RootComponent);
	MarkWidget->SetVisibility(false);

	SoundComponent = CreateDefaultSubobject<USounds_F35>(TEXT("SoundComponent"));

	RadarDetectCollsion = CreateDefaultSubobject<USphereComponent>(TEXT("RadarDetectCollsion"));
	RadarDetectCollsion->SetupAttachment(RootComponent);

	RadarComponent = CreateDefaultSubobject<URadarComponent>(TEXT("RadarComponent"));

	AutoCannonClass = AAutoCannon::StaticClass();
}

void AFighter::BeginPlay()
{
	Super::BeginPlay();

	if (AutoCannonClass)
	{
		AutoCannon = GetWorld()->SpawnActor<AAutoCannon>(AutoCannonClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (AutoCannon)
		{
			AutoCannon->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AutoCannonSocketName);
			AutoCannon->SetOwner(this);
		}
	}

	OriginalSpringArmLength = MainCameraSpringArm->TargetArmLength;

	SoundComponent->SetStartupOptions(true, true, 68600.f);
	SoundComponent->InitialiseAllSounds();
	if (IsLocallyControlled())
	{
		if (!SoundComponent->GetIsCockpitMixerOn())
		{
			SoundComponent->SwitchCockpitSnd();
		}
	}
	else
	{
		APawn* LocalContorlledPawn = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->GetPawn();
		AFighter* LocalContorlledFighter = Cast<AFighter>(LocalContorlledPawn);
		if (LocalContorlledFighter)
		{
			this->SoundComponent->ChangeAudioListener(LocalContorlledFighter->GetMainCamera());
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("ChangeAudioListener to:") + LocalContorlledFighter->GetName());
		}
	}
}

void AFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SynchroMovement(DeltaTime);

	HandleRudderInput(DeltaTime);

	UpdateThrusterFX(DeltaTime);

	VisionUpdate(DeltaTime);

	SoundComponentUpdate(DeltaTime);
}

void AFighter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFighter, bInRedTeam);
	DOREPLIFETIME(AFighter, TargetLocation);
	DOREPLIFETIME(AFighter, TargetRotation);
}

void AFighter::SynchroMovement(float DeltaTime)
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

void AFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ThisClass::LookUpDown);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ThisClass::LookRightLeft);

	PlayerInputComponent->BindAxis(TEXT("Thruster"), this, &ThisClass::ThrusterInput);
	PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &ThisClass::PitchInput);
	PlayerInputComponent->BindAxis(TEXT("Roll"), this, &ThisClass::RollInput);
	PlayerInputComponent->BindAxis(TEXT("RightRudder"), this, &ThisClass::RightRudderInput);
	PlayerInputComponent->BindAxis(TEXT("LeftRudder"), this, &ThisClass::LeftRudderInput);

	PlayerInputComponent->BindAction(TEXT("Flap"), EInputEvent::IE_Pressed, this, &ThisClass::FlapBttnPressed);
	PlayerInputComponent->BindAction(TEXT("WheelRetreat"), EInputEvent::IE_Pressed, this, &ThisClass::WheelRetreatBttnPressed);
	PlayerInputComponent->BindAction(TEXT("AutoCannon"), EInputEvent::IE_Pressed, this, &ThisClass::AutoCannonBttnPressed);
	PlayerInputComponent->BindAction(TEXT("AutoCannon"), EInputEvent::IE_Released, this, &ThisClass::AutoCannonBttnReleased);
}

void AFighter::LookUpDown(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("LookUp: %f"), Value));
	VisionInput.Y = Value;
}

void AFighter::LookRightLeft(float Value)
{
	VisionInput.X = Value;
}

void AFighter::ThrusterInput(float Value)
{
	ServerThrusterInput(Value);
}

void AFighter::PitchInput(float Value)
{
	ServerPitchInput(Value);
}

void AFighter::RollInput(float Value)
{
	ServerRollInput(Value);
}

void AFighter::RightRudderInput(float Value)
{
	ServerRightRudderInput(Value);
}

void AFighter::LeftRudderInput(float Value)
{
	ServerLeftRudderInput(Value);
}

void AFighter::FlapBttnPressed()
{
	ServerFlapBttnPressed();
}

void AFighter::WheelRetreatBttnPressed()
{
	ServerWheelRetreatBttnPressed();
}

void AFighter::AutoCannonBttnPressed()
{
	ServerAutoCannonBttnPressed();
	if (AutoCannon)
	{
		AutoCannon->FireStart();
	}
}

void AFighter::AutoCannonBttnReleased()
{
	ServerAutoCannonBttnReleased();
	if (AutoCannon)
	{
		AutoCannon->FireEnd();
	}
}

void AFighter::ServerThrusterInput_Implementation(float Value)
{
	AeroPhysicsComponent->SetAddThruster(Value);
}

void AFighter::ServerPitchInput_Implementation(float Value)
{
	AeroPhysicsComponent->SetAeroPitchControl(Value);
}

void AFighter::ServerRollInput_Implementation(float Value)
{
	AeroPhysicsComponent->SetAeroRollControl(Value);
}

void AFighter::ServerRightRudderInput_Implementation(float Value)
{
	RightRudderInputVal = Value;
}

void AFighter::ServerLeftRudderInput_Implementation(float Value)
{
	LeftRudderInputVal = Value;
}

void AFighter::ServerFlapBttnPressed_Implementation()
{
	AeroPhysicsComponent->SetAeroFlapActivated(!AeroPhysicsComponent->GetFlapControlActivated());
}

void AFighter::ServerWheelRetreatBttnPressed_Implementation()
{
	AeroPhysicsComponent->SetWheelsRetreated(!AeroPhysicsComponent->GetIsWheelsRetreated());
}

void AFighter::ServerAutoCannonBttnPressed_Implementation()
{
	if (AutoCannon)
	{
		AutoCannon->FireStart();
	}
}

void AFighter::ServerAutoCannonBttnReleased_Implementation()
{
	if (AutoCannon)
	{
		AutoCannon->FireEnd();
	}
}

void AFighter::HandleRudderInput(float DeltaTime)
{
	if (HasAuthority())
	{
		if (RightRudderInputVal > 0.005f && LeftRudderInputVal > 0.05f)
		{
			float input = (LeftRudderInputVal + RightRudderInputVal) * 0.5f;
			AeroPhysicsComponent->SetWheelsBrake(input);
			AeroPhysicsComponent->SetAeroYawControl(0.0f);
			AeroPhysicsComponent->SetSteeringWheels(0.0f);
		}
		else
		{
			AeroPhysicsComponent->SetWheelsBrake(0.0f);
			if (RightRudderInputVal > LeftRudderInputVal)
			{
				AeroPhysicsComponent->SetSteeringWheels(RightRudderInputVal);
				AeroPhysicsComponent->SetAeroYawControl(RightRudderInputVal);
			}
			else
			{
				AeroPhysicsComponent->SetSteeringWheels(-LeftRudderInputVal);
				AeroPhysicsComponent->SetAeroYawControl(-LeftRudderInputVal);
			}
		}
	}
}

void AFighter::UpdateThrusterFX(float DeltaTime)
{
	float ThrusterRatio = AeroPhysicsComponent->GetRealThrusterRatio();

	float AfterBurnerThreshold = AeroPhysicsComponent->GetAfterBurnerThresholdRatio();

	// DistortionSize Value Calculate
	float DistortionSize = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(30.0f, 65.0f), ThrusterRatio) * ThrusterFXConfig.DistortionSize;

	FLinearColor OutEmissive = ThrusterFXConfig.EmissiveOuter;
	FLinearColor InerEmissive = ThrusterFXConfig.EmissiveIner;

	float EmissiveOutStrength = 0.0f;
	float EmissiveInerStrength = 0.0f;
	float ThrusterRatioScale = 0.0f;
	float TargetTrasitionVal = 0.0f;
	if (ThrusterRatio > AfterBurnerThreshold)
	{
		ThrusterRatioScale = FMath::GetMappedRangeValueClamped(FVector2D(AfterBurnerThreshold, 1.0f), FVector2D(0.6, 1.0f), ThrusterRatio);
		TargetTrasitionVal = 1.0f;
	}

	ThrusterFXTrasition = FMath::FInterpTo(ThrusterFXTrasition, TargetTrasitionVal, DeltaTime, 0.8f);

	FVector JetScale = FVector(1.1f, 0.15f, 0.15f) * ThrusterFXConfig.FlameBodyScale * ThrusterRatioScale * ThrusterFXTrasition;
	FVector JetRingsScale = FVector(10.5f, 10.5f, 13.5f) * ThrusterFXConfig.RingScale * ThrusterRatioScale * ThrusterFXTrasition;
	float RingOpacity = 0.8f * ThrusterRatioScale * ThrusterFXTrasition;

	EmissiveOutStrength = ThrusterFXConfig.EmissiveOuter.A * ThrusterRatioScale * ThrusterFXTrasition;
	EmissiveInerStrength = ThrusterFXConfig.EmissiveIner.A * ThrusterRatioScale * ThrusterFXTrasition;

	OutEmissive.A = EmissiveOutStrength;
	InerEmissive.A = EmissiveInerStrength;

	if (ThrusterFXLeft && ThrusterFXLeft->IsActive())
	{
		ThrusterFXLeft->SetFloatParameter(FName("DistortionSize"), DistortionSize);
		ThrusterFXLeft->SetVectorParameter(FName("JetScale"), JetScale);
		ThrusterFXLeft->SetVectorParameter(FName("JetRingsScale"), JetRingsScale);
		ThrusterFXLeft->SetFloatParameter(FName("RingOpacity"), RingOpacity);
		ThrusterFXLeft->SetColorParameter(FName("EmissiveOuter"), OutEmissive);
		ThrusterFXLeft->SetColorParameter(FName("EmissiveIner"), InerEmissive);
	}
	if (ThrusterFXRight && ThrusterFXRight->IsActive())
	{
		ThrusterFXRight->SetFloatParameter(FName("DistortionSize"), DistortionSize);
		ThrusterFXRight->SetVectorParameter(FName("JetScale"), JetScale);
		ThrusterFXRight->SetVectorParameter(FName("JetRingsScale"), JetRingsScale);
		ThrusterFXRight->SetFloatParameter(FName("RingOpacity"), RingOpacity);
		ThrusterFXRight->SetColorParameter(FName("EmissiveOuter"), OutEmissive);
		ThrusterFXRight->SetColorParameter(FName("EmissiveIner"), InerEmissive);
	}
}

void AFighter::VisionUpdate(float DeltaTime)
{
	FQuat UpDown = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(VisionInput.Y * 70.0f));
	FQuat RightLeft = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(VisionInput.X * 150.0f)); 

	SpringArmQuat = FMath::QInterpTo(SpringArmQuat, RightLeft * UpDown, DeltaTime, 6.5f);

	// Change TargetArmLength depend on Acceleration
	FVector Acceleration = AeroPhysicsComponent->GetCurrentAcceleration();
	float ForwardAcceleration = Acceleration.Dot(GetActorForwardVector());
	float ForwardRatio = FMath::GetMappedRangeValueClamped(FVector2D(-1200.0f, 1200.0f), FVector2D(-1.0f, 1.0f), ForwardAcceleration);

	// Change MainCamera Pitch Rotation depend on GFroce
	float TargetPitchRatio = FMath::GetMappedRangeValueClamped(FVector2D(-3.0f, 3.0f), FVector2D(-1.0f, 1.0f), AeroPhysicsComponent->GetCurrentGForce());
	MainCameraPitchRatio = FMath::FInterpTo(MainCameraPitchRatio, TargetPitchRatio, DeltaTime, 0.7f);
	FQuat MainCameraPitchQuat = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(-MainCameraPitchRatio * 13.0f));

	MainCameraSpringArm->SetRelativeRotation(SpringArmQuat);
	MainCameraSpringArm->TargetArmLength = FMath::FInterpTo(MainCameraSpringArm->TargetArmLength, OriginalSpringArmLength + ForwardRatio * 400.0f, DeltaTime, 1.0f);
	//MainCameraSpringArm->SocketOffset

	MainCamera->SetRelativeRotation(MainCameraPitchQuat);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("Forward A: %f"), ForwardAcceleration));
}

void AFighter::SoundComponentUpdate(float DeltaTime)
{
	SoundComponent->SetRPM(AeroPhysicsComponent->GetRealThrusterRatio());
	SoundComponent->SetSpeed(AeroPhysicsComponent->GetCurrentGroundSpeed() / 0.036f);
	SoundComponent->SetPitchRate(AeroPhysicsComponent->GetCurrentGForce());
	SoundComponent->SetIsEngineRunning(true);
	SoundComponent->SetIsPlaneOnLand(GetActorLocation().Z < 500.0f);

	FSoundParams_F35 SoundPara = SoundComponent->SoundParams;

	SoundComponent->UpdatePlaneSounds(SoundPara);
}

void AFighter::SetMarkWidgetVisble(bool bIsVisible)
{
	MarkWidget->SetVisibility(bIsVisible);
}



