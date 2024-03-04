// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_SkyDominion.h"
#include "Kismet/GameplayStatics.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "GameFramework/PlayerStart.h"


AActor* AGM_SkyDominion::ChoosePlayerStart_Implementation(AController* Player)
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
   
    int index = 0;
    ASkyPlayerState* PlayerState = Player->GetPlayerState<ASkyPlayerState>();
    if (PlayerState)
    {
        if (!PlayerState->bInRedTeam)
        {
            index += 2;
        }
        index += PlayerState->TeamIndex;
    }

    FName StartTag;
    switch (index)
    {
    case 0:
        StartTag = TEXT("Red_1");
        break;

    case 1:
        StartTag = TEXT("Red_2");
        break;

    case 2:
        StartTag = TEXT("Blue_1");
        break;

    case 3:
        StartTag = TEXT("Blue_2");
        break;
    }

    for (auto PlayerStart : PlayerStarts)
    {
        if (PlayerStart->ActorHasTag(StartTag))
        {
            return PlayerStart;
        }
    }
    return nullptr;
}
