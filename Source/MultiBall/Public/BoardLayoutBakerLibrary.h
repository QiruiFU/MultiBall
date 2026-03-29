// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BoardLayoutDataAsset.h"
#include "BoardLayoutBakerLibrary.generated.h"

/**
 * Utility library to help bake level layout data into data assets.
 */
UCLASS()
class MULTIBALL_API UBoardLayoutBakerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Scans the provided World for PlaceableActors and bakes them into the specified Round Index of the TargetAsset.
	 * Highly useful in Editor Utility Widgets for WYSIWYG layout design.
	 * 
	 * @param ContextWorld The world to scan (typically the editor world).
	 * @param TargetAsset  The BoardLayoutDataAsset to save the layout into.
	 * @param RoundIndex   The index of the round (0 = first round).
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor|Layout")
	static void BakeLevelToDataAsset(UWorld* ContextWorld, UBoardLayoutDataAsset* TargetAsset, int32 RoundIndex);
};
