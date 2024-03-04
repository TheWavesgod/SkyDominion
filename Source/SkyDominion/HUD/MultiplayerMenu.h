// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerMenu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	int SessionIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Session Info")
	FString SessionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Info")
	int32 VacantNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Info")
	int32 PingInMs;
};

/**
 * 
 */
UCLASS()
class SKYDOMINION_API UMultiplayerMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer")
	void UpdateSessionList();

	UFUNCTION(BlueprintCallable)
	void JoinSession(FSessionInfo SessionToJoin);

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UPROPERTY(BlueprintReadOnly)
	TArray<FSessionInfo> SessionSearchResults;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* Bttn_HostGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Bttn_SearchGame;

	UFUNCTION()
	void HostGameClicked();

	UFUNCTION()
	void SearchGameClicked();

	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	
};
