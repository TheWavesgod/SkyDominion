#pragma once

#include "CoreMinimal.h"
#include "AerodynamicPhysics/Public/Airplane.h"
#include "SkyDominion/Interface/RadarInterface.h"

#include "Fighter.generated.h"

class USoundCue;

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

USTRUCT(BlueprintType)
struct FAlertSoundConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* LowAltitudeAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* RWSBeScanedAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* TargetLockingAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* TargetLockedAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* VTBeScanedAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* STTBeLockedAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* MissileComingAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* MissileComingCloseAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* UnvalidMoveAlert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundCue* TargetLostAlert;
};

/**
 * 
 */
UCLASS()
class SKYDOMINION_API AFighter : public AAirplane, public IRadarInterface
{
	GENERATED_BODY()

public:
	AFighter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category = "ThrusterFX")
	FCustomThrusterParameter ThrusterFXConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	TSubclassOf<class AAutoCannon> AutoCannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	FName AutoCannonSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Config")
	FName FighterDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Config")
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Config")
	FAlertSoundConfig AlertSoundConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Config")
	TSubclassOf<class AFighterWreckage> WreckageClass;

	UPROPERTY(Replicated)
	bool bInRedTeam;

	void SetMarkWidgetVisble(bool bIsVisible);

	/** For alert */
	void ActivateAlertSoundLowAltitude(bool bActivated);

	UFUNCTION(BlueprintCallable)
	void ActiveUnvalidMoveAlertSound();

	void ActiveTargetLostAlertSound();

	void ActiveTargetLockingSound();

	void ActiveTargetLockedSound();

	void ShutDownRadarLockSound();

	virtual void MissileComingWarning(bool bIsClose) override;

	UFUNCTION(Client, Reliable)
	void ClientMissileComingAlert(bool bIsClose);

	UFUNCTION(Client, Reliable)
	void ActivateRwsBeScanedAlert();

	UFUNCTION(Client, Reliable)
	void ActivateVTBeScanedAlert();

	UFUNCTION(Client, Reliable)
	void ActivateSTTBeLockedAlert();

	UFUNCTION(Client, Reliable)
	void DeactivateSTTBeLockedAlert();

	void ShowNoTargetAlert();
	void ShowMissileOutofRange();
	void ShowTargetLost();
	void ShowTargetLocked();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Elim();

	virtual void Destroyed() override;

	/** Timer for Radar RWS Mode */
	float RWSDetectTimer = 0.0f;
	float RWSDisapearTimer = 0.0f;
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
	class UNiagaraComponent* TailFXLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UNiagaraComponent* TailFXRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USounds_F35* SoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	class UWidgetComponent* MarkWidget;

	/** Component For Simulate Radar */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* RadarDetectCollsion;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class URadarComponent* RadarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UMissileComponent* MissileComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	AAutoCannon* AutoCannon;

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
	void AutoCannonBttnPressed();
	void AutoCannonBttnReleased();
	void ChangeMissileBttnPressed();
	void FireMissileBttnPressed();
	void ChangeRadarModeBttnPressed();
	void LockBttnPressed();

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

	UFUNCTION(Server, Reliable)
	void ServerAutoCannonBttnPressed();

	UFUNCTION(Server, Reliable)
	void ServerAutoCannonBttnReleased();

	UFUNCTION(Server, Reliable)
	void ServerChangeMissileBttnPressed();

	UFUNCTION(Server, Reliable)
	void ServerFireMissileBttnPressed();


	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InvestigatorController, AActor* DamageCauser);

	UFUNCTION()
	void OnFighterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY()
	class ASkyPlayerState* SkyPlayerState;

	void CheckPlayerStateInfo();

	/** Replicated Fighter Movement */
	UPROPERTY(Replicated)
	FVector TargetLocation;

	UPROPERTY(Replicated)
	FQuat TargetRotation;

	void SynchroMovement(float DeltaTime);

	/** Fighter Jet Health */
	UPROPERTY(Replicated)
	float CurrentHealth;

	void HandleRudderInput(float DeltaTime);

	float RightRudderInputVal = 0.0f;
	float LeftRudderInputVal = 0.0f;

	float ThrusterFXTrasition = 0.0f;
	void UpdateThrusterFX(float DeltaTime);

	void UpdateTailFX(float DeltaTime);

	/** Fighter Vision Update */
	void VisionUpdate(float DeltaTime);

	FVector2D VisionInput;
	FQuat SpringArmQuat;
	float MainCameraPitchRatio = 0.0f;
	float OriginalSpringArmLength;

	/** Fighter Sound Update */
	void SoundComponentUpdate(float DeltaTime);

	/** Alert Sound Component */
	class UAudioComponent* LowAltitudeHandle;
	UAudioComponent* RadarLockHandle;
	UAudioComponent* VTBeScanedHandle;
	UAudioComponent* STTBeLockedHandle;
	UAudioComponent* MissileComingHandle;

	/** HUD */
	class UPlayerOverlay* PlayerOverlay;

	/** Missile Info */
	UPROPERTY(Replicated)
	FName MissileName;

	UPROPERTY(Replicated)
	int32 MissileNum;

	UPROPERTY(Replicated)
	float MissileRange;

	void SyncMissileInfo();

	/** Update Alert */
	void UpdateAlertState(float DeltaTime);

	const float MissileComingAlertTimeGap = 0.5f;
	float MissileComingAlertHandle = 0.0f;
	bool bIsMissileClose = false;

	const float MissileNotCloseWarningTimeGap = 1.8f;
	float MissileNotCloseWarningTimeHandle = 0.0f;

public:
	FORCEINLINE USounds_F35* GetSoundComponent() const { return SoundComponent; }
	FORCEINLINE UCameraComponent* GetMainCamera() const { return MainCamera; }
	FORCEINLINE USphereComponent* GetRadarDetectCollision() const { return RadarDetectCollsion; }
	FORCEINLINE UWidgetComponent* GetMarkWidget() const { return MarkWidget; }
	FORCEINLINE URadarComponent* GetRadarComponent() const { return RadarComponent; }
	FORCEINLINE AAutoCannon* GetAutoCannon() const { return AutoCannon; }
	FORCEINLINE float GetCurrentHealthPercent() const { return CurrentHealth / MaxHealth; }
	FORCEINLINE FName GetCurrentMissileName() const { return MissileName; }
	FORCEINLINE int32 GetCurrentMissileNum() const { return MissileNum; }
	FORCEINLINE float GetCurrentMissileRange() const { return MissileRange; }
	int GetAutoCannonBulletLeft() const;
	void SetPlayerOverlay(UPlayerOverlay* val) { PlayerOverlay = val; }
};
