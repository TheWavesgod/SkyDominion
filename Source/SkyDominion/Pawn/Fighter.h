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


	/** PlayerInput Bind Function */
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
	
};
