// Copyright Autonomix. All Rights Reserved.

#include "MultiBallGameMode.h"
#include "Player/MultiBallPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AMultiBallGameMode::AMultiBallGameMode()
{
	// Use our custom PlayerController class
	PlayerControllerClass = AMultiBallPlayerController::StaticClass();

	// Set default pawn class to our custom pawn class
	// DefaultPawnClass = ADefaultPawn::StaticClass();
}
