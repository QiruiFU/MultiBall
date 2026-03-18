// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiBallTypes.h"
#include "MultiBallPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/**
 * Replicated player state tracking coins, score, inventory, and round data.
 */
UCLASS()
class MULTIBALL_API AMultiBallPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMultiBallPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Current gold balance. */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Economy")
	int32 PlayerCoins;

	/** Score data for the current round. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	int32 RoundChips;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	float RoundMultiplier;

	/** Cumulative total score across all rounds. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	int64 TotalScore;

	/** Current round number from this player's perspective. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Round")
	int32 CurrentRound;

	// --- Inventory ---

	/** Purchased but not yet placed items. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryEntry> Inventory;

	/** Broadcast whenever inventory changes. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	/** Add one unit of a placeable to inventory. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddToInventory(TSubclassOf<APlaceableActor> PlaceableClass);

	/** Remove one unit from inventory. Returns false if not found. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveFromInventory(TSubclassOf<APlaceableActor> PlaceableClass);

	/** Check how many of a placeable the player owns. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetInventoryCount(TSubclassOf<APlaceableActor> PlaceableClass) const;
};
