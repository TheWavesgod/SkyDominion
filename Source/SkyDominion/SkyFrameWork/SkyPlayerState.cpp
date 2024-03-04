// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyPlayerState.h"
#include "Net/UnrealNetwork.h"

void ASkyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASkyPlayerState, bInRedTeam);
	DOREPLIFETIME(ASkyPlayerState, TeamIndex);
}
