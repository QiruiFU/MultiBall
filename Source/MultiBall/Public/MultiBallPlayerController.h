// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlaceableActor.h"
#include "UFloatingScoreWidget.h"
#include "MultiBallTypes.h"

#include "MultiBallPlayerController.generated.h"


class UNotificationWidget;
class UPhaseButtonWidget;
class USpecialSkillWidget;
class UGameOverWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpacebarAction, bool, bIsPressed);

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

    /** Store a durability override to be applied when this class is next placed. */
    UFUNCTION(BlueprintCallable, Category = "Shop")
    void SetPendingDurability(TSubclassOf<APlaceableActor> PlaceableClass, int32 MaxDurability);

    UFUNCTION(Server, Reliable, WithValidation)
    void PurchasePlaceable(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location, FVector Offset);

    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    TSubclassOf<APlaceableActor> SelectedPlaceableClass;

    /** Broadcasts spacebar pressed state for game mechanics (like Flippers). */
    UPROPERTY(BlueprintAssignable, Category = "Input")
    FOnSpacebarAction OnSpacebarAction;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaTime) override;

    void HandlePlacementClick();
    void HandleSpacebarPressed();
    void HandleSpacebarReleased();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ShopWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> BuildWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UNotificationWidget> NotificationWidgetClass;

    /** Show a large centered notification. */
    void ShowNotification(const FString& Message, float Duration = 2.0f, FLinearColor Color = FLinearColor::White);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UPhaseButtonWidget> PhaseButtonWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<USpecialSkillWidget> SpecialSkillWidgetClass;



    UFUNCTION()
    void HandlePhaseChanged(EGamePhase NewPhase);

private:
    void DebugEnterShop();
    void DebugEnterDrop();
    void DebugCheatWin();

    /** Toggle flip direction of the ghost preview (R key). */
    void HandleFlipPressed();

    /** Whether the ghost preview is currently flipped. */
    bool bGhostFlipped;

    UPROPERTY()
    UUserWidget* BuildWidget;

    // --- Ghost Preview ---

    /** Spawn a translucent ghost of the selected placeable. */
    void SpawnGhostPreview();

    /** Destroy the current ghost preview actor. */
    void DestroyGhostPreview();

    /** Update ghost position to follow mouse cursor each frame. */
    void UpdateGhostPreview();

    /** Check if a location is valid to place the current item. */
    bool IsPlacementValid(const FVector& Location) const;

    UPROPERTY()
    APlaceableActor* GhostPreviewActor;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> GhostMaterials;

    /** Durability overrides from shop items, keyed by placeable class. */
    UPROPERTY()
    TMap<TSubclassOf<APlaceableActor>, int32> PendingDurabilityOverrides;

    UPROPERTY()
    UNotificationWidget* NotificationWidgetInstance;

    UPROPERTY()
    UPhaseButtonWidget* PhaseButtonWidgetInstance;

    UPROPERTY()
    USpecialSkillWidget* SpecialSkillWidgetInstance;

    UPROPERTY()
    class URemainingBallsWidget* RemainingBallsWidgetInstance;

    UPROPERTY()
    class UGameOverWidget* GameOverWidgetInstance;

    UPROPERTY()
    UUserWidget* FloatingScoreWidgetInstance;

    UFUNCTION()
    void OnSkillSelectedFromUI(ESpecialSkill ChosenSkill);
};
