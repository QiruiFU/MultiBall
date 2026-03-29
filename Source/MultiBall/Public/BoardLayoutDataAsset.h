// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MultiBallTypes.h"
#include "BoardLayoutDataAsset.generated.h"

/**
 * Data asset containing per-round fixed board layouts.
 * Create instances in the editor and assign to the GameMode.
 * Index 0 = Round 1, Index 1 = Round 2, etc.
 * If a round exceeds the array, no fixed components spawn.
 */
UCLASS()
class MULTIBALL_API UBoardLayoutDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Ordered list of round layouts. Index 0 = Round 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout")
	TArray<FRoundLayout> RoundLayouts;
};
