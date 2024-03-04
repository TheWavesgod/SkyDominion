#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Airplane.generated.h"

UCLASS()
class AERODYNAMICPHYSICS_API AAirplane : public APawn
{
	GENERATED_BODY()

public:
	AAirplane();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UAeroPhysicsComponent* AeroPhysicsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* Mesh;

private:

public:	
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
};
