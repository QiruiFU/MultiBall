// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MultiBallTypes.h"
#include "ScoreSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundScoreUpdated, FScoreData, NewScore);

/**
 * World subsystem that accumulates per-round scoring
 * using the Chips x Multiplier model.
 */
UCLASS()
class MULTIBALL_API UScoreSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Add a single ball's finalised score to the round total. */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddBallScore(int32 Chips, float Multiplier);

	/** Get the current round score data. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	FScoreData GetRoundScore() const;

	/** Reset the round score for a new round. */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void ResetRoundScore();

	/** Check if the player's round score beats the opponent. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	bool DidPlayerWin(int64 OpponentScore) const;

	/** Get the actual final score including all end-of-round multiplier skills. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	int64 GetFinalCalculatedScore() const;

	/** Broadcast whenever the round score changes. */
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnRoundScoreUpdated OnRoundScoreUpdated;

private:
	FScoreData RoundScore;
};
