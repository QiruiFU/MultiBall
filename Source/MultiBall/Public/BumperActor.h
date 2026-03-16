// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "BumperActor.generated.h"

/**
 * A bouncy bumper that applies a physics impulse to balls on hit
 * and adds a high multiplier to the score.
 */
UCLASS()
class MULTIBALL_API ABumperActor : public APlaceableActor
{
	GENERATED_BODY()

public:
	ABumperActor();

	/** Force applied to balls on collision. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bumper")
	float BounceForce;

	/** Override to apply impulse on hit. */
	virtual void OnBallHit(ABallActor* Ball) override;
};
