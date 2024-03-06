#pragma once

#include "CoreMinimal.h"
#include "AerodynamicPhysics/Public/Airplane.h"
#include "Fighter.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API AFighter : public AAirplane
{
	GENERATED_BODY()

public:
	AFighter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USpringArmComponent* MainCameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* MainCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UNiagaraComponent* ThrusterFXLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UNiagaraComponent* ThrusterFXRight;

	/** PlayerInput Bind Function */
	void LookUpDown(float Value);
	void LookRightLeft(float Value);
	void ThrusterInput(float Value);
	void PitchInput(float Value);
	void RollInput(float Value);
	void RightRudderInput(float Value);
	void LeftRudderInput(float Value);
	void FlapBttnPressed();
	void WheelRetreatBttnPressed();

	UFUNCTION(Server, Unreliable)
	void ServerThrusterInput(float Value);

	UFUNCTION(Server, Unreliable)
	void ServerPitchInput(float Value);

	UFUNCTION(Server, Unreliable)
	void ServerRollInput(float Value);

	UFUNCTION(Server, Unreliable)
	void ServerRightRudderInput(float Value);

	UFUNCTION(Server, Unreliable)
	void ServerLeftRudderInput(float Value);

	UFUNCTION(Server, Reliable)
	void ServerFlapBttnPressed();

	UFUNCTION(Server, Reliable)
	void ServerWheelRetreatBttnPressed();

private:
	void HandleRudderInput(float DeltaTime);

	float RightRudderInputVal = 0.0f;
	float LeftRudderInputVal = 0.0f;

	float ThrusterFXTrasition = 0.0f;
	void UpdateThrusterFX(float DeltaTime);

	/** Fighter Vision Update */
	void VisionUpdate(float DeltaTime);

	FVector2D VisionInput;
	FQuat SpringArmQuat;
	float OriginalSpringArmLength;
};
