// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiBallTypes.h"
#include "MultiBallPlayerState.generated.h"

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
};
