// Copyright SOUNDFX STUDIO © 2022

#include "F35SoundSystem.h"

#define LOCTEXT_NAMESPACE "FF35SoundSystemModule"

void FF35SoundSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FF35SoundSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FF35SoundSystemModule, F35SoundSystem)