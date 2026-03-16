// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiBallGameMode.generated.h"

/**
 * The GameMode defines the game being played. It governs the game rules, scoring, and dictates the spawning of Pawns.
 */
UCLASS()
class MULTIBALL_API AMultiBallGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiBallGameMode();
};
