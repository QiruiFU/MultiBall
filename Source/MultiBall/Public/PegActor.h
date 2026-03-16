// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "PegActor.generated.h"

/**
 * A standard peg that adds chips to balls on collision.
 */
UCLASS()
class MULTIBALL_API APegActor : public APlaceableActor
{
	GENERATED_BODY()

public:
	APegActor();

	/** Override for peg-specific hit behavior (e.g. visual flash). */
	virtual void OnBallHit(ABallActor* Ball) override;

protected:
	virtual void BeginPlay() override;
};
