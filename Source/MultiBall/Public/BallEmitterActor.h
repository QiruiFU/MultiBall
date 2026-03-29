// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallEmitterActor.generated.h"

class ABallActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllBallsFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallsRemainingChanged, int32, BallsRemaining);

/**
 * Spawns balls during the Drop phase with configurable
 * movement pattern and timing.
 */
UCLASS()
class MULTIBALL_API ABallEmitterActor : public AActor
{
	GENERATED_BODY()

public:
	ABallEmitterActor();

	/** Start the drop sequence with the given number of balls. */
	UFUNCTION(BlueprintCallable, Category = "Emitter")
	void StartDropSequence(int32 NumBalls);

	/** Stop the current drop sequence. */
	UFUNCTION(BlueprintCallable, Category = "Emitter")
	void StopDropSequence();

	/** Manually drop a single ball from user input. */
	UFUNCTION(BlueprintCallable, Category = "Emitter")
	void ManualDropBall();

	/** Drop a single ball immediately (internal/sequence). */
	UFUNCTION(BlueprintCallable, Category = "Emitter")
	void DropBall();

	/** Destroy all active balls in the world (for round cleanup). */
	UFUNCTION(BlueprintCallable, Category = "Emitter")
	void ClearAllBalls();

	/** Broadcast when all balls in the sequence have settled or been destroyed. */
	UPROPERTY(BlueprintAssignable, Category = "Emitter")
	FOnAllBallsFinished OnAllBallsFinished;

	/** Broadcast when the number of remaining balls changes. */
	UPROPERTY(BlueprintAssignable, Category = "Emitter")
	FOnBallsRemainingChanged OnBallsRemainingChanged;

	/** Get the number of balls remaining to be dropped. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Emitter")
	int32 GetBallsRemaining() const { return BallsRemaining; }

	// --- Configuration ---

	/** If true, the emitter waits for ManualDropBall() instead of auto-dropping on a timer. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter")
	bool bManualDrop;

	/** Ball class to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter")
	TSubclassOf<ABallActor> BallClass;

	/** Time between each ball drop in a sequence. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter")
	float DropInterval;

	/** Total balls per round. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter")
	int32 BallsPerRound;

	// --- Movement Pattern ---

	/** Horizontal oscillation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OscillationSpeed;

	/** Horizontal oscillation range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OscillationRange;

	// --- Visual ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* EmitterMesh;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnBallDestroyed(AActor* DestroyedActor);

	FVector OriginLocation;
	float OscillationTime;

	// Drop sequence state
	bool bIsDropping;
	int32 BallsRemaining;
	float DropTimer;

	// Track active balls
	int32 ActiveBallCount;
};
