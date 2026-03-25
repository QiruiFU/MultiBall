// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiBallTypes.h"
#include "Components/WidgetComponent.h"
#include "UFloatingScoreWidget.h"
#include "PlaceableActor.generated.h"

class ABallActor;
class USphereComponent;

UCLASS()
class MULTIBALL_API APlaceableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APlaceableActor();

	// --- Shop / Economy ---

	/** Gold cost to purchase this component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 Cost;

	/** Category of this placeable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placeable")
	EPlaceableType PlaceableType;

	// --- Scoring ---

	/** Chip value added to a ball on hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	int32 ChipValue;

	/** Multiplier value applied to a ball on hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	float MultiplierValue;

	// --- Durability ---

	/** Maximum hits before this placeable breaks. 0 = indestructible. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	int32 MaxDurability;

	/** Current remaining hits. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Durability")
	int32 CurrentDurability;

	/** Whether this placeable is broken and no longer scores. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Durability")
	bool bIsBroken;

	// --- Collision ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PlacementBlockingRadius;

	UPROPERTY()
	UWidgetComponent* WidgetComponent;

	UPROPERTY()

	UUFloatingScoreWidget* WidgetInstance;

	// --- Scoring Interface ---

	/** Called when a ball hits this placeable. Override in subclasses for custom behavior. */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	virtual void OnBallHit(ABallActor* Ball);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};