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

	/** Whether this flipper is mirrored (faces the other direction). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Flipper")
	bool bIsFlipped;

	/** Apply or remove the horizontal flip. */
	UFUNCTION(BlueprintCallable, Category = "Flipper")
	void SetFlipped(bool bFlipped);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Called when the player controller broadcasts spacebar state changes. */
	UFUNCTION()
	void OnSpacebarChanged(bool bIsPressed);

private:
	float CurrentAngle;
	float TargetAngle;

public:
	/** The rotation at spawn time (or updated by ghost preview). */
	FQuat BaseSpawnRotation;

private:

	/** Internal helper to apply/remove visual mirror on the mesh. */
	void ApplyFlipVisual();
};
