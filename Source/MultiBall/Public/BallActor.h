// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "MultiBallTypes.h"
#include <Sound/SoundBase.h>
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

	/** Multiply the current multiplier from a placeable hit. */
	void MultiplyMultiplier(float Amount);

	/** Get current accumulated score data. */
	FScoreData GetScoreData() const;

	/** Starting chips count. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	int32 BaseChips;

	/** Starting multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	float BaseMultiplier;

	/** Cooldown for hitting the same object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionCooldown;

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

	/**
	 * Override for split chance.  -1 = use global skill value (native ball).
	 * Split balls receive parent_chance * 0.5 so each generation splits less.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball")
	float SplitChanceOverride;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Audio)
	UAudioComponent* MyAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* HitSound; // or USoundCue*

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
	float timesHit;

	/** Tracks the last time this ball hit a specific actor to enforce cooldown. */
	TMap<AActor*, float> LastHitTimes;
};
