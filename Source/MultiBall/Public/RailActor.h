// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "RailActor.generated.h"

class USplineComponent;

/**
 * A guide rail that channels ball movement along a spline path.
 * Place in the editor and adjust control points to shape the rail.
 */
UCLASS()
class MULTIBALL_API ARailActor : public APlaceableActor
{
	GENERATED_BODY()

public:
	ARailActor();

	/** Spline defining the rail path. Edit control points in the editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineComponent* SplineComponent;

	/** Force applied to guide the ball along the spline. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail")
	float GuideForce;

	/** Override to apply guiding force along the spline. */
	virtual void OnBallHit(ABallActor* Ball) override;
};
