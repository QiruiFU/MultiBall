// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "FlipperActor.generated.h"

/**
 * A flipper that rotates upwards when the spacebar is pressed to knock balls away.
 */
UCLASS()
class MULTIBALL_API AFlipperActor : public APlaceableActor
{
	GENERATED_BODY()
	
public:
	AFlipperActor();

	/** The angle the flipper rests at. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flipper")
	float MinAngle;

	/** The maximum angle the flipper pops up to when activated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flipper")
	float MaxAngle;

	/** How fast the flipper rotates (degrees per second). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flipper")
	float FlapSpeed;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Called when the player controller broadcasts spacebar state changes. */
	UFUNCTION()
	void OnSpacebarChanged(bool bIsPressed);

private:
	float CurrentAngle;
	float TargetAngle;
	FQuat BaseSpawnRotation;
};
