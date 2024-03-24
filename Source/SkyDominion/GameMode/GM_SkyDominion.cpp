// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_SkyDominion.h"
#include "Kismet/GameplayStatics.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameState.h"
#include "GameFramework/DefaultPawn.h"
#include "SkyDominion/SkyFrameWork/SkyGameInstance.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "SkyDominion/Pawn/SkySpectatorPawn.h"

void AGM_SkyDominion::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ASkyPlayerState* NewPlayerState = NewPlayer->GetPlayerState<ASkyPlayerState>();
    if (NewPlayerState)
    {
        int PlayerId = NewPlayerState->GetPlayerId();
        FString TeamType = NewPlayerState->bInRedTeam ? FString("Red") : FString("Blue");
        //GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("NewLogin Id: %d Choose Team: "), PlayerId) + TeamType);
    }
}

AActor* AGM_SkyDominion::ChoosePlayerStart_Implementation(AController* Player)
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
   
    
    int index = 0;
    ASkyPlayerState* PlayerState = Player->GetPlayerState<ASkyPlayerState>();
    APlayerState* originPlayerState = Player->GetPlayerState<APlayerState>();

    for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
    {
        if (originPlayerState == GameState->PlayerArray[i].Get())
        {
            index = i;
        }
    }

	/*if (PlayerState)
	{
		if (!PlayerState->bInRedTeam)
		{
			index += 2;
		}
		index += PlayerState->TeamIndex;
	}*/

    FName StartTag;
    switch (index)
    {
    case 0:
        StartTag = TEXT("Red_1");
        break;

    case 1:
        StartTag = TEXT("Blue_1");
        break;

    case 2:
        StartTag = TEXT("Red_2");
        break;

    case 3:
        StartTag = TEXT("Blue_2");
        break;
    }

	/* if (GEngine)
	 {
		 GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Play %d choose"), index) + StartTag.ToString());
	 }*/

    for (auto PlayerStart : PlayerStarts)
    {
        if (PlayerStart->ActorHasTag(StartTag))
        {
            return PlayerStart;
        }
    }
    return nullptr;
}

UClass* AGM_SkyDominion::GetDefaultPawnClassForController_Implementation(AController* InController)
{
#if WITH_EDITOR && DO_CHECK
    UClass* DefaultClass = DefaultPawnClass.DebugAccessRawClassPtr();
    if (DefaultClass)
    {
        if (FBlueprintSupport::IsClassPlaceholder(DefaultClass))
        {
            ensureMsgf(false, TEXT("Trying to spawn class that is, directly or indirectly, a placeholder"));
            return ADefaultPawn::StaticClass();
        }
    }
#endif
    int index = 0;
    APlayerState* originPlayerState = InController->GetPlayerState<APlayerState>();

    for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
    {
        if (originPlayerState == GameState->PlayerArray[i].Get())
        {
            index = i;
        }
    }
    /** Change Team Set */
    ASkyPlayerState* PlayerState = InController->GetPlayerState<ASkyPlayerState>();
    if (index % 2 == 1)
    {
        PlayerState->bInRedTeam = false;
    }

    USkyGameInstance* GameInstance = GetGameInstance<USkyGameInstance>();
    if (GameInstance->PlayersChooseJetList.IsEmpty())
    {
        return DefaultPawnClass;
    }
    if (GameInstance)
    {
        int JetIndex = GameInstance->PlayersChooseJetList[index];

        UClass* PawnClass = GameInstance->FighterJetClass[JetIndex];
        if (PawnClass)
        {
            return PawnClass;
        }
        else
        {
            return DefaultPawnClass;
        }
    }
    return DefaultPawnClass;
}

void AGM_SkyDominion::FighterDestroyed(AFighter* DestroyedFighter, ASkyPlayerController* VictimController, ASkyPlayerController* AttackerController)
{
    ASkyPlayerState* VictimPlayerState = VictimController ? Cast<ASkyPlayerState>(VictimController->PlayerState) : nullptr;
    ASkyPlayerState* AttackerPlayerState = AttackerController ? Cast<ASkyPlayerState>(AttackerController->PlayerState) : nullptr;

    if (VictimPlayerState && AttackerPlayerState)
    {
        if (VictimPlayerState->bInRedTeam != AttackerPlayerState->bInRedTeam)
        {
            AttackerPlayerState->AddKill();
        }
        VictimPlayerState->AddDefeat();
    }

    if (DestroyedFighter)
    {
        DestroyedFighter->Elim();
    }
}

void AGM_SkyDominion::SwitchToSpectator(AFighter* DestroyedFighter, ASkyPlayerController* SpecController)
{
    if (!DestroyedFighter || !SpecController) return;
    
    FTransform SpecSpawnTransform;
    SpecSpawnTransform.SetLocation(DestroyedFighter->GetActorLocation());
    FRotator FighterRotation = DestroyedFighter->GetActorRotation();
    FighterRotation.Roll = 0.0f;
    SpecSpawnTransform.SetRotation(FQuat(FighterRotation));

    DestroyedFighter->Reset();
    DestroyedFighter->Destroy();

    //if (SpectatorClass) return;
	SpecController->ChangeState(NAME_Spectating);
    SpecController->SetSpectatorState(true);
    SpecController->SetControlRotation(FighterRotation);
    GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Change to Spectator Pawn."));
}

void AGM_SkyDominion::RequestRespawn(ASkyPlayerController* TargetController)
{
    if (!TargetController) return;

    TargetController->ChangeState(NAME_Playing);
    TargetController->SetSpectatorState(false);

	/* GetDefaultPawnClassForController(TargetController)->StaticClass();

	 FTransform SpawnTransform = ChoosePlayerStart(TargetController)->GetActorTransform();

	 AFighter* newFighter = GetWorld()->SpawnActor<AFighter>(GetDefaultPawnClassForController(TargetController)->StaticClass(), SpawnTransform, FActorSpawnParameters());*/

    RestartPlayer(TargetController);
}




