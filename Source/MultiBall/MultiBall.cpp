// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiBall.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "LayoutEditor/SMultiBallLayoutBakerWindow.h"
#endif

void FMultiBallModule::StartupModule()
{
#if WITH_EDITOR
	SMultiBallLayoutBakerWindow::RegisterTabSpawner();
#endif
}

void FMultiBallModule::ShutdownModule()
{
#if WITH_EDITOR
	SMultiBallLayoutBakerWindow::UnregisterTabSpawner();
#endif
}

IMPLEMENT_PRIMARY_GAME_MODULE( FMultiBallModule, MultiBall, "MultiBall" );
