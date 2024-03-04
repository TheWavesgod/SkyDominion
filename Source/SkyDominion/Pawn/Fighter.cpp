// Fill out your copyright notice in the Description page of Project Settings.


#include "Fighter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AerodynamicPhysics/public/AeroPhysicsComponent.h"

AFighter::AFighter()
{
	PrimaryActorTick.bCanEverTick = true;

	MainCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainCameraSpringArm"));
	MainCameraSpringArm->SetupAttachment(RootComponent);

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(MainCameraSpringArm);
}

void AFighter::BeginPlay()
{
	Super::BeginPlay();
}

void AFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Thruster"), this, &ThisClass::ThrusterInput);
	PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &ThisClass::PitchInput);
	PlayerInputComponent->BindAxis(TEXT("Roll"), this, &ThisClass::RollInput);
	PlayerInputComponent->BindAxis(TEXT("RightRudder"), this, &ThisClass::RightRudderInput);
	PlayerInputComponent->BindAxis(TEXT("LeftRudder"), this, &ThisClass::LeftRudderInput);

	PlayerInputComponent->BindAction(TEXT("Flap"), EInputEvent::IE_Pressed, this, &ThisClass::FlapBttnPressed);
	PlayerInputComponent->BindAction(TEXT("WheelRetreat"), EInputEvent::IE_Pressed, this, &ThisClass::WheelRetreatBttnPressed);
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
}

void AFighter::ServerLeftRudderInput_Implementation(float Value)
{
}

void AFighter::ServerFlapBttnPressed_Implementation()
{
	
}

void AFighter::ServerWheelRetreatBttnPressed_Implementation()
{
	AeroPhysicsComponent->SetWheelsRetreated(!AeroPhysicsComponent->GetIsWheelsRetreated());
}
