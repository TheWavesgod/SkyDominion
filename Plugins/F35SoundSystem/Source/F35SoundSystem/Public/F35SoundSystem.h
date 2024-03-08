// Copyright SOUNDFX STUDIO © 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FF35SoundSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
