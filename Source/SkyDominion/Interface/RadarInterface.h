// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RadarInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URadarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKYDOMINION_API IRadarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void MissileComingWarning(bool bIsClose);

	// between 0 ~ 100
	virtual float GetHeatIndex() const;
};
