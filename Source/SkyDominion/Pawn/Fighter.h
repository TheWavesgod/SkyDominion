#pragma once

#include "CoreMinimal.h"
#include "AerodynamicPhysics/Public/Airplane.h"
#include "Fighter.generated.h"

USTRUCT(BlueprintType)
struct FCustomThrusterParameter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	float DistortionSize = 1.0f;

	// X for flame length, y z for flame radius
	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	FVector FlameBodyScale = FVector(1.0f, 1.0f, 1.0f);

	// Z for rings length, x y for rings radius
	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	FVector RingScale = FVector(1.0f, 1.0f, 1.0f);

	// RGB for the emissive outer Color, A for the Maximum emissive strength
	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	FLinearColor EmissiveOuter;

	// RGB for the emissive iner Color, A for the Maximum emissive strength
	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	FLinearColor EmissiveIner;
};

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	FCustomThrusterParameter ThrusterFXConfig;

	UPROPERTY(Replicated)
	bool bInRedTeam;

	void SetMarkWidgetVisble(bool bIsVisible);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USounds_F35* SoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UWidgetComponent* MarkWidget;

	/** Component For Simulate Radar */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* RadarDetectCollsion;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class URadarComponent* RadarComponent;

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
	float MainCameraPitchRatio = 0.0f;
	float OriginalSpringArmLength;

	/** Fighter Sound Update */
	void SoundComponentUpdate(float DeltaTime);

public:
	FORCEINLINE USounds_F35* GetSoundComponent() const { return SoundComponent; }
	FORCEINLINE UCameraComponent* GetMainCamera() const { return MainCamera; }
	FORCEINLINE USphereComponent* GetRadarDetectCollision() const { return RadarDetectCollsion; }
	FORCEINLINE UWidgetComponent* GetMarkWidget() const { return MarkWidget; }
	FORCEINLINE URadarComponent* GetRadarComponent() const { return RadarComponent; }
};
