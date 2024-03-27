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
#include "SkyDominion/HUD/PlayerOverlay.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "SkyDominion/Actor/FighterWreckage.h"
#include "SkyDominion/GameMode/GM_SkyDominion.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "SkyDominion/HUD/SkyDominionHUD.h"
#include "SkyDominion/Actor/MissileComponent.h"

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

	MissileComponent = CreateDefaultSubobject<UMissileComponent>(TEXT("MissileComponent"));

	AutoCannonClass = AAutoCannon::StaticClass();

	CurrentHealth = MaxHealth;

	LowAltitudeHandle = CreateDefaultSubobject<UAudioComponent>(TEXT("LowAltitudeHandle"));
	LowAltitudeHandle->bAutoActivate = false;
	LowAltitudeHandle->bStopWhenOwnerDestroyed = true;
}

void AFighter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

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
		SoundComponent->SetIsFlybyCamera(true);
	}

	if (HasAuthority())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Mesh->SetNotifyRigidBodyCollision(true);
		Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnFighterHit);

		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);

		if (MissileComponent)
		{
			MissileComponent->SpawnMissileAtBegin(RootComponent);
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
	DOREPLIFETIME(AFighter, CurrentHealth);
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
	PlayerInputComponent->BindAction(TEXT("ChangeMissile"), EInputEvent::IE_Pressed, this, &ThisClass::ChangeMissileBttnPressed);
	PlayerInputComponent->BindAction(TEXT("FireMissile"), EInputEvent::IE_Pressed, this, &ThisClass::FireMissileBttnPressed);
}

void AFighter::Elim_Implementation()
{
	if (AutoCannon)
	{
		AutoCannon->FireEnd();
		AutoCannon->Destroy();
	}

	if (MissileComponent)
	{
		MissileComponent->DestroyMissileRemain();
	}

	ASkyPlayerController* OnwingController = Cast<ASkyPlayerController>(Controller);
	if (OnwingController)
	{
		ASkyDominionHUD* SkyDominionHUD = OnwingController->GetHUD<ASkyDominionHUD>();
		if (SkyDominionHUD)
		{
			SkyDominionHUD->OwnerFighter = nullptr;
			SkyDominionHUD->SetPlayerOverlayVisibility(false);
			SkyDominionHUD->AddSpectatorOverlay();
		}
	}

	AGM_SkyDominion* SkyDominionGM = GetWorld()->GetAuthGameMode<AGM_SkyDominion>();
	if (SkyDominionGM && OnwingController)
	{
		SkyDominionGM->SwitchToSpectator(this, OnwingController);
	}
}

void AFighter::Destroyed()
{
	Super::Destroyed();

	if (HasAuthority() && WreckageClass)
	{
		AFighterWreckage* Wreckage = GetWorld()->SpawnActor<AFighterWreckage>(WreckageClass, GetActorTransform(), FActorSpawnParameters());
		Wreckage->SetPhysicalVelocity(Mesh->GetPhysicsLinearVelocity(), Mesh->GetPhysicsAngularVelocityInRadians());
	}
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

		if (!PlayerOverlay)
		{
			ASkyPlayerController* OnwingController = Cast<ASkyPlayerController>(Controller);
			if (OnwingController)
			{
				ASkyDominionHUD* SkyDominionHUD = OnwingController->GetHUD<ASkyDominionHUD>();
				if (SkyDominionHUD)
				{
					PlayerOverlay = SkyDominionHUD->PlayerOverlay;
				}
			}
		}
		if (PlayerOverlay)
		{
			PlayerOverlay->ShowBulletRunoutAlert();
		}
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

void AFighter::ChangeMissileBttnPressed()
{
	ServerChangeMissileBttnPressed();
}

void AFighter::FireMissileBttnPressed()
{
	ServerFireMissileBttnPressed();
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

void AFighter::ServerChangeMissileBttnPressed_Implementation()
{
	if (MissileComponent)
	{
		MissileComponent->ChangeSelectMissile();
	}
}

void AFighter::ServerFireMissileBttnPressed_Implementation()
{
}


void AFighter::HandleRudderInput(float DeltaTime)
{
	if (HasAuthority())
	{
		if (RightRudderInputVal > 0.005f && LeftRudderInputVal > 0.005f)
		{
			float input = (LeftRudderInputVal + RightRudderInputVal) * 0.5f;
			AeroPhysicsComponent->SetWheelsBrake(input);
			AeroPhysicsComponent->SetAirbrake(input);
			AeroPhysicsComponent->SetAeroYawControl(0.0f);
			AeroPhysicsComponent->SetSteeringWheels(0.0f);
		}
		else
		{
			AeroPhysicsComponent->SetWheelsBrake(0.0f);
			AeroPhysicsComponent->SetAirbrake(0.0f);
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
	FQuat UpDown = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(VisionInput.Y * 80.0f));
	FQuat RightLeft = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(VisionInput.X * 170.0f)); 

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

	//MainCamera->SetRelativeRotation(MainCameraPitchQuat);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("Forward A: %f"), ForwardAcceleration));
}

void AFighter::SoundComponentUpdate(float DeltaTime)
{
	if (IsLocallyControlled())
	{
		if (!SoundComponent->bInitializeComplete)
			SoundComponent->SwitchCockpitSnd();
		if (!SoundComponent->GetIsCockpitMixerOn())
		{
			SoundComponent->SwitchCockpitSnd();
			SoundComponent->SetIsFlybyCamera(false);
		}
	}

	SoundComponent->SetRPM(AeroPhysicsComponent->GetRealThrusterRatio());
	SoundComponent->SetSpeed(AeroPhysicsComponent->GetCurrentGroundSpeed() / 0.036f);
	SoundComponent->SetPitchRate(AeroPhysicsComponent->GetCurrentGForce());
	SoundComponent->SetIsEngineRunning(true);
	SoundComponent->SetIsPlaneOnLand(GetActorLocation().Z < 500.0f);

	SoundComponent->SoundParams.bPlayCockpitSounds = IsLocallyControlled() ? true : false;

	SoundComponent->UpdatePlaneSounds(SoundComponent->SoundParams);

	/*if (IsLocallyControlled())
		SoundComponent->UpdatePlaneSounds(SoundComponent->SoundParams);
	else
		SoundComponent->UpdateFlybyPlaneSounds(SoundComponent->SoundParams);*/
}

void AFighter::SetMarkWidgetVisble(bool bIsVisible)
{
	MarkWidget->SetVisibility(bIsVisible);
}


void AFighter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InvestigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	if (CurrentHealth == 0.0f)
	{
		AGM_SkyDominion* SkyDominionGM = GetWorld()->GetAuthGameMode<AGM_SkyDominion>();
		if (SkyDominionGM)
		{
			ASkyPlayerController* OwnerController = Cast<ASkyPlayerController>(Controller);
			ASkyPlayerController* AttackerController = Cast<ASkyPlayerController>(InvestigatorController);

			SkyDominionGM->FighterDestroyed(this, OwnerController, AttackerController);
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Receive Damage by ") + InvestigatorController->GetName());
}

void AFighter::OnFighterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Hit ") + OtherActor->GetName() + FString::Printf(TEXT(" Impulse: %d"), NormalImpulse.Size()));
	float HitNormalVel = Mesh->GetPhysicsLinearVelocity().ProjectOnTo(Hit.ImpactNormal).Size() * 0.036;

	float Damage = HitNormalVel * 0.5;

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	if (CurrentHealth == 0.0f)
	{
		ASkyPlayerController* OwnerController = GetController<ASkyPlayerController>();
		if (OwnerController && OwnerController->GetPlayerState<ASkyPlayerState>())
			OwnerController->GetPlayerState<ASkyPlayerState>()->AddDefeat();

		Elim();
	}
}

int AFighter::GetAutoCannonBulletLeft() const
{
	if (AutoCannon)
	{
		return AutoCannon->GetCurrentBulletLeft();
	}
	return 0;
}

void AFighter::ActivateAlertSoundLowAltitude(bool bActivated)
{
	if (bActivated)
	{
		LowAltitudeHandle->SetSound(AlertSoundConfig.LowAltitudeAlert);
		LowAltitudeHandle->Play();
	}
	if (!bActivated && LowAltitudeHandle->IsPlaying())
	{
		LowAltitudeHandle->Stop();
	}
}

