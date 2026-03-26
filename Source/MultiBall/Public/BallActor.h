// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiBallTypes.h"
#include "BallActor.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBallScoreFinalized, int32, Chips, float, Multiplier);

/**
 * A physics ball that accumulates score (Chips x Multiplier)
 * as it collides with placeables during the Drop phase.
 */
UCLASS()
class MULTIBALL_API ABallActor : public AActor
{
	GENERATED_BODY()

public:
	ABallActor();

	// --- Scoring ---

	/** Add chips from a placeable hit. */
	void AddChips(int32 Amount);

	/** Add multiplier from a placeable hit. */
	void AddMultiplier(float Amount);

	/** Get current accumulated score data. */
	FScoreData GetScoreData() const;

	/** Broadcast when the ball settles and finalizes its score. */
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnBallScoreFinalized OnBallScoreFinalized;

	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// --- Physics Settings ---

	/** Velocity threshold below which the ball is considered settled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	float SettleVelocityThreshold;

	/** Time the ball must stay below threshold to be considered settled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	float SettleTimeRequired;

	/** Z height below which the ball is killed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	float KillZ;

	/** Maximum time this ball stays alive before being forcibly settled and destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	float MaxLifespan;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	/** Called when lifespan expires. */
	UFUNCTION()
	void OnLifespanExpired();

	/** Finalize score and destroy the ball. */
	void SettleBall();

	int32 AccumulatedChips;
	float AccumulatedMultiplier;
	float SettleTimer;
	bool bHasSettled;
	FTimerHandle LifespanTimerHandle;
};
