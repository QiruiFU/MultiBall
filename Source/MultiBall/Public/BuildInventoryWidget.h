// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiBallTypes.h"
#include "BuildInventoryWidget.generated.h"

class APlaceableActor;
class UHorizontalBox;
class UButton;
class UTextBlock;

/**
 * Inventory bar displayed at the bottom of the screen.
 * Shows purchased items during both Shop and Build phases.
 */
UCLASS()
class MULTIBALL_API UBuildInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Refresh the inventory display from the player's state. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

	/** Called when the player clicks an item in the inventory. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnItemSelected(TSubclassOf<APlaceableActor> PlaceableClass);

protected:
	virtual void NativeConstruct() override;

	/** Horizontal container for inventory items. Bind in Blueprint. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, OptionalWidget), Category = "Inventory")
	UHorizontalBox* InventoryContainer;

private:
	/** Maps buttons to their corresponding placeable class. */
	UPROPERTY()
	TMap<UButton*, TSubclassOf<APlaceableActor>> ButtonToClassMap;

	/** Single click handler that identifies which button was pressed. */
	UFUNCTION()
	void HandleInventoryButtonClicked();
};
