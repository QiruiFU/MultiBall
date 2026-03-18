// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlaceableActor.h"
#include "MultiBallTypes.h"
#include "MultiBallPlayerController.generated.h"

class UNotificationWidget;

/**
 * Player controller handling shop, placement, and ghost preview.
 */
UCLASS()
class MULTIBALL_API AMultiBallPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AMultiBallPlayerController();

    UFUNCTION(BlueprintCallable, Category = "Shop")
    void SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass);

    UFUNCTION(Server, Reliable, WithValidation)
    void PurchasePlaceable(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location);

    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    TSubclassOf<APlaceableActor> SelectedPlaceableClass;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaTime) override;

    void HandlePlacementClick();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ShopWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> BuildWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UNotificationWidget> NotificationWidgetClass;

    /** Show a large centered notification. */
    void ShowNotification(const FString& Message, float Duration = 2.0f);

    UFUNCTION()
    void HandlePhaseChanged(EGamePhase NewPhase);

private:
    void DebugEnterShop();
    void DebugEnterBuild();
    void DebugEnterDrop();

    UPROPERTY()
    UUserWidget* BuildWidget;

    // --- Ghost Preview ---

    /** Spawn a translucent ghost of the selected placeable. */
    void SpawnGhostPreview();

    /** Destroy the current ghost preview actor. */
    void DestroyGhostPreview();

    /** Update ghost position to follow mouse cursor each frame. */
    void UpdateGhostPreview();

    UPROPERTY()
    APlaceableActor* GhostPreviewActor;

    UPROPERTY()
    UNotificationWidget* NotificationWidgetInstance;
};
