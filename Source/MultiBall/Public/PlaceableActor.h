// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiBallTypes.h"
#include "Components/WidgetComponent.h"
#include "UFloatingScoreWidget.h"
#include "DurabilityBarWidget.h"
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

	/** If true, this was placed by the system (fixed layout) and cannot be moved/sold. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placeable")
	bool bIsFixed;

	/**
	 * If > 0, this actor was placed in the editor as a fixed layout component.
	 * It will only be visible/active during the specified round number.
	 * 0 = player-placed at runtime (default).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	int32 FixedForRound;

	// --- Scoring ---

	// Scoring is now handled via attached UInteractionRuleComponent objects.

	// --- Durability ---

	/** Maximum hits before this placeable breaks. 0 = indestructible. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	int32 MaxDurability;

	/** Set durability at runtime (creates health bar if needed). Call after spawn. */
	UFUNCTION(BlueprintCallable, Category = "Durability")
	void InitDurability(int32 NewMaxDurability);

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

	// --- Durability Bar ---

	UPROPERTY()
	UWidgetComponent* DurabilityBarComp;

	UPROPERTY()
	UDurabilityBarWidget* DurabilityBarInstance;

	// --- Scoring Interface ---

	/** Called when a ball hits this placeable. Override in subclasses for custom behavior. */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	virtual void OnBallHit(ABallActor* Ball);

protected:
	virtual void BeginPlay() override;

	/** Refresh the durability bar fill and visibility. */
	void UpdateDurabilityBar();

public:	
	virtual void Tick(float DeltaTime) override;
};