// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultiBallTypes.h"
#include "MultiBallBlueprintLibrary.generated.h"

UCLASS()
class MULTIBALL_API UMultiBallBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "MultiBall")
	static FVector GetLocationFromHitResult(const FHitResult& HitResult);

	// --- Score helpers (for WBP_ShopUI binding) ---

	/** Returns the current round's accumulated score data (Chips + Multiplier). */
	UFUNCTION(BlueprintPure, Category = "MultiBall|Score", meta = (WorldContext = "WorldContextObject"))
	static FScoreData GetCurrentRoundScore(const UObject* WorldContextObject);

	/** Returns the current round's total score as formatted text (e.g. "1,234"). */
	UFUNCTION(BlueprintPure, Category = "MultiBall|Score", meta = (WorldContext = "WorldContextObject"))
	static FText GetTotalScoreFormatted(const UObject* WorldContextObject);

	/** Returns the current round number. */
	UFUNCTION(BlueprintPure, Category = "MultiBall|Score", meta = (WorldContext = "WorldContextObject"))
	static int32 GetCurrentRoundNumber(const UObject* WorldContextObject);

	/** Returns the player's current coin balance. */
	UFUNCTION(BlueprintPure, Category = "MultiBall|Score")
	static int32 GetPlayerCoins(APlayerController* PC);

	/** Returns the current opponent's target score the player must beat. */
	UFUNCTION(BlueprintPure, Category = "MultiBall|Score", meta = (WorldContext = "WorldContextObject"))
	static int64 GetOpponentTargetScore(const UObject* WorldContextObject);
};
