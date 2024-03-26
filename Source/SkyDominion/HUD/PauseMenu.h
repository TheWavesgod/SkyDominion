// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class SKYDOMINION_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* Bttn_ResumeGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Bttn_MainMenu;

	UFUNCTION()
	void ResumeGameBtnClicked();

	UFUNCTION()
	void MainMenuBtnClicked();
};
