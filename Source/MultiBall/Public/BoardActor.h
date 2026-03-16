// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardActor.generated.h"

class UBoxComponent;
class APlaceableActor;

/**
 * Defines the play-area bounds and validates placement positions.
 */
UCLASS()
class MULTIBALL_API ABoardActor : public AActor
{
	GENERATED_BODY()

public:
	ABoardActor();

	/** Check if a location is valid for placing a component. */
	UFUNCTION(BlueprintCallable, Category = "Board")
	bool IsLocationValid(FVector Location, TSubclassOf<APlaceableActor> PlaceableClass) const;

	/** Register a placed actor. */
	UFUNCTION(BlueprintCallable, Category = "Board")
	void RegisterPlaceable(APlaceableActor* Placeable);

	/** Remove a placed actor from the registry. */
	UFUNCTION(BlueprintCallable, Category = "Board")
	void UnregisterPlaceable(APlaceableActor* Placeable);

	/** Remove all placed actors. */
	UFUNCTION(BlueprintCallable, Category = "Board")
	void ClearBoard();

	/** Get all placed actors. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Board")
	const TArray<APlaceableActor*>& GetPlacedActors() const { return PlacedActors; }

	// --- Components ---

	/** Play-area bounding box. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoardBounds;

	/** Visual representation of the board. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoardMesh;

	// --- Configuration ---

	/** Minimum distance between placed actors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board")
	float MinPlacementDistance;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<APlaceableActor*> PlacedActors;
};
