// Copyright Autonomix. All Rights Reserved.

#include "MultiBallGameMode.h"
#include "MultiBallPlayerState.h"
#include "MultiBallPlayerController.h"

AMultiBallGameMode::AMultiBallGameMode()
{
    PlayerStateClass = AMultiBallPlayerState::StaticClass();
    PlayerControllerClass = AMultiBallPlayerController::StaticClass();
}
