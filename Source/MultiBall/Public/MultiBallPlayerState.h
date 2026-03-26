// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiBallTypes.h"
#include "MultiBallPlayerState.generated.h"

class APlaceableActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/**
 * Replicated player state tracking coins, score, and round data.
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



	/** Cumulative total score across all rounds. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	int64 TotalScore;

	/** Current round number from this player's perspective. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Round")
	int32 CurrentRound;

	// --- Inventory ---

	/** Items the player has purchased but not yet placed. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryEntry> Inventory;

	/** Broadcast when inventory changes. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	/** Add an item to the inventory. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddToInventory(TSubclassOf<APlaceableActor> PlaceableClass);

	/** Remove one item from the inventory. Returns false if not found. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveFromInventory(TSubclassOf<APlaceableActor> PlaceableClass);

	/** Get the count of a specific item in the inventory. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetInventoryCount(TSubclassOf<APlaceableActor> PlaceableClass) const;
};
