// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlaceableActor.h"
#include "MultiBallTypes.h"
#include "MultiBallPlayerController.generated.h"

class ABoardActor;
class UBuildInventoryWidget;

/**
 * Player controller handling placement, ghost preview, and debug controls.
 */
UCLASS()
class MULTIBALL_API AMultiBallPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AMultiBallPlayerController();

    // --- Shop / Placement ---

    UFUNCTION(BlueprintCallable, Category = "Shop")
    void SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass);

    /** Buy an item during Shop phase: deducts coins and adds to inventory. Call from Shop UI. */
    UFUNCTION(BlueprintCallable, Category = "Shop")
    bool BuyPlaceable(TSubclassOf<APlaceableActor> PlaceableClass);

    /** Place an item during Build phase: consumes from inventory and spawns actor. */
    UFUNCTION(Server, Reliable, WithValidation)
    void PurchasePlaceable(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location);

    UPROPERTY(BlueprintReadWrite, Setter = SetSelectedPlaceableClass, Category = "Shop")
    TSubclassOf<APlaceableActor> SelectedPlaceableClass;

    /** Native setter - called automatically when Blueprint sets SelectedPlaceableClass. */
    void SetSelectedPlaceableClass(TSubclassOf<APlaceableActor> NewClass);

    // --- Ghost Preview ---

    /** Spawn/destroy/update the translucent ghost following the cursor. */
    void SpawnGhostPreview();
    void DestroyGhostPreview();
    void UpdateGhostPreview();

    /** Called when game phase changes. */
    UFUNCTION()
    void HandlePhaseChanged(EGamePhase NewPhase);

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaTime) override;

    void HandlePlacementClick();

    // --- UI ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ShopWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> BuildWidgetClass;

private:
    // Debug key bindings for testing game loop
    void DebugEnterShop();
    void DebugEnterBuild();
    void DebugEnterDrop();

    // --- Ghost state ---
    UPROPERTY()
    APlaceableActor* GhostPreviewActor;

    bool bIsPlacementValid;

    /** Track changes to SelectedPlaceableClass from Blueprint. */
    UPROPERTY()
    TSubclassOf<APlaceableActor> LastSelectedClass;

    // Cached references
    UPROPERTY()
    ABoardActor* CachedBoardActor;

    // Build UI
    UPROPERTY()
    UUserWidget* BuildWidget;

    /** Translucent green material for valid placement. */
    UPROPERTY()
    UMaterialInterface* GhostValidMaterial;

    /** Translucent red material for invalid placement. */
    UPROPERTY()
    UMaterialInterface* GhostInvalidMaterial;
};
