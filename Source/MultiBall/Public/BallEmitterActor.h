// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallEmitterActor.generated.h"

class ABallActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllBallsFinished);

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

	/** Manually drop a single ball. */
	UFUNCTION(BlueprintCallable, Category = "Emitter")
	void DropBall();

	/** Broadcast when all balls in the sequence have settled or been destroyed. */
	UPROPERTY(BlueprintAssignable, Category = "Emitter")
	FOnAllBallsFinished OnAllBallsFinished;

	// --- Configuration ---

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
