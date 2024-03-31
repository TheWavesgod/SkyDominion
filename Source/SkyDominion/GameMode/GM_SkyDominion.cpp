// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_SkyDominion.h"
#include "Kismet/GameplayStatics.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "SkyDominion/SkyFrameWork/SkyGameState.h"
#include "GameFramework/DefaultPawn.h"
#include "SkyDominion/SkyFrameWork/SkyGameInstance.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "SkyDominion/Pawn/SkySpectatorPawn.h"

void AGM_SkyDominion::StartPlay()
{
    Super::StartPlay();

    StartRoundTimer(60 * 20);
}

void AGM_SkyDominion::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ASkyPlayerState* NewPlayerState = NewPlayer->GetPlayerState<ASkyPlayerState>();
    if (NewPlayerState)
    {
        USkyGameInstance* GameInstance = GetGameInstance<USkyGameInstance>();

        TSharedPtr<const FUniqueNetId> PlayerId = NewPlayerState->GetUniqueId().GetUniqueNetId();

        if (GameInstance->PlayersInfoList.Find(PlayerId))
        {
            FPlayersInfo Info = *(GameInstance->PlayersInfoList.Find(PlayerId));

            NewPlayerState->bInRedTeam = Info.bInRedTeam;
            NewPlayerState->TeamIndex = Info.TeamIndex;
            NewPlayerState->ChoosedFighterType = static_cast<EFighterJetType>(Info.ChooseJet);

            FString JetType;
            switch (NewPlayerState->ChoosedFighterType)
            {
            case EFighterJetType::E_F15E:
                JetType = TEXT("F15");
                break;

            case EFighterJetType::E_Su33:
                JetType = TEXT("Su33");
                break;

            case EFighterJetType::E_F18C:
                JetType = TEXT("F18");
                break;
            }
            GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("NewPlayer ID: ") + PlayerId->ToString() + FString(" Choose ") + JetType);
        }

        int playerNum = GameState->PlayerArray.Num();
        if (playerNum % 2 != 1)
        {
            NewPlayerState->bInRedTeam = false;
        }
        if (playerNum > 2)
        {
            NewPlayerState->TeamIndex = 1;
        }


        //FString TeamType = NewPlayerState->bInRedTeam ? FString("Red") : FString("Blue");
        //GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("NewLogin Id: %d Choose Team: "), playerNum) + TeamType);
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

    ASkyPlayerState* PlayerState = InController->GetPlayerState<ASkyPlayerState>();
    USkyGameInstance* GameInstance = GetGameInstance<USkyGameInstance>();

    if (PlayerState && GameInstance)
    {
        if (PlayerState->ChoosedFighterType == EFighterJetType::E_Max)
            return DefaultPawnClass;

        UClass* PawnClass = GameInstance->FighterJetClass[static_cast<int>(PlayerState->ChoosedFighterType)];

        if (PawnClass)
        {
            return PawnClass;
        }
        else
        {
            return DefaultPawnClass;
        }
    }

	/*if (index % 2 == 1)
	{
		PlayerState->bInRedTeam = false;
	}*/

	/*USkyGameInstance* GameInstance = GetGameInstance<USkyGameInstance>();
	if (GameInstance->PlayersInfoList.IsEmpty())
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
	}*/


    return DefaultPawnClass;
}

void AGM_SkyDominion::FighterDestroyed(AFighter* DestroyedFighter, ASkyPlayerController* VictimController, ASkyPlayerController* AttackerController)
{
    ASkyPlayerState* VictimPlayerState = VictimController ? Cast<ASkyPlayerState>(VictimController->PlayerState) : nullptr;
    ASkyPlayerState* AttackerPlayerState = AttackerController ? Cast<ASkyPlayerState>(AttackerController->PlayerState) : nullptr;

    ASkyGameState* SkyGameState = GetGameState<ASkyGameState>();

    if (VictimPlayerState && AttackerPlayerState && SkyGameState)
    {
        if (VictimPlayerState->bInRedTeam != AttackerPlayerState->bInRedTeam)
        {
            AttackerPlayerState->AddKill();
            
            AttackerPlayerState->bInRedTeam ? SkyGameState->AddScore_RedTeam() : SkyGameState->AddScore_BlueTeam();
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
    //SpecController->SetAudioListenerOverride(SpecController->GetSpectatorPawn()->FindComponentByClass<USceneComponent>(), FVector(0), FRotator(0));
    //GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Change to Spectator Pawn."));
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

void AGM_SkyDominion::StartRoundTimer(int RoundTime)
{
    LeftRoundTime = RoundTime;
    GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ThisClass::CountdownTick, 1.0f, true);
}

void AGM_SkyDominion::CountdownTick()
{
    --LeftRoundTime;

    ASkyGameState* SkyGameState = GetGameState<ASkyGameState>();
    if (SkyGameState) SkyGameState->SetRoundTime(LeftRoundTime);

    if (LeftRoundTime <= 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
    }
}




