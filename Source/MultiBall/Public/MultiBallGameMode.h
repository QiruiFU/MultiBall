// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiBallTypes.h"
#include "MultiBallGameMode.generated.h"

class ABallEmitterActor;
class UShopComponent;
class UOpponentDataAsset;
class ABoardActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundComplete, int32, Round, bool, bPlayerWon);

/**
 * Central orchestrator for the Shop -> Build -> Drop -> Rewards game loop.
 */
UCLASS()
class MULTIBALL_API AMultiBallGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiBallGameMode();

	// --- Phase Transitions ---

	UFUNCTION(BlueprintCallable, Category = "Game Loop")
	void EnterShopPhase();

	UFUNCTION(BlueprintCallable, Category = "Game Loop")
	void EnterBuildPhase();

	UFUNCTION(BlueprintCallable, Category = "Game Loop")
	void EnterDropPhase();

	UFUNCTION(BlueprintCallable, Category = "Game Loop")
	void EnterRewardsPhase();

	// --- Accessors ---

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Loop")
	EGamePhase GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Loop")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Loop")
	int64 GetCurrentOpponentTargetScore() const { return CurrentOpponent.TargetScore; }

	// --- Events ---

	UPROPERTY(BlueprintAssignable, Category = "Game Loop")
	FOnPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Loop")
	FOnRoundComplete OnRoundComplete;

	// --- Configuration ---

	/** Reference to the ball emitter in the level. Set in editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Loop")
	ABallEmitterActor* BallEmitter;

	/** Opponent data asset containing the roster. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Loop")
	UOpponentDataAsset* OpponentRoster;

	/** Reference to the board actor in the level. Auto-found if not set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Loop")
	ABoardActor* BoardActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Loop")
	int32 MaxRounds;

	/** Coins awarded for winning a round. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 WinRewardCoins;

	/** Starting coins for the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 StartingCoins;

	/** Maximum time for the Drop phase before auto-transitioning to Rewards. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Loop")
	float DropPhaseDuration;

	// --- Shop ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	UShopComponent* ShopComponent;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnAllBallsFinished();

	EGamePhase CurrentPhase;
	int32 CurrentRound;
	FOpponentData CurrentOpponent;
	FTimerHandle DropPhaseTimerHandle;
};
