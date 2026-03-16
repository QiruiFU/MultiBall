// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MultiBallTypes.h"
#include "OpponentDataAsset.generated.h"

/**
 * Data asset containing the roster of preset opponents.
 * Create instances in the editor and assign to the GameMode.
 */
UCLASS()
class MULTIBALL_API UOpponentDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Ordered list of opponents the player faces. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Opponents")
	TArray<FOpponentData> Opponents;
};
