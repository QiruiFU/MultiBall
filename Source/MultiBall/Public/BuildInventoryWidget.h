// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiBallTypes.h"
#include "BuildInventoryWidget.generated.h"

class APlaceableActor;
class UVerticalBox;
class UButton;
class UTextBlock;

/**
 * C++ base class for the Build-phase inventory sidebar.
 * Displays purchased items and lets the player select one for placement.
 * Designed to be subclassed in Blueprint for visual layout.
 */
UCLASS()
class MULTIBALL_API UBuildInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Refresh the inventory display from the player's state. */
	UFUNCTION(BlueprintCallable, Category = "Build")
	void RefreshInventory();

	/** Called when the player clicks an item in the inventory. */
	UFUNCTION(BlueprintCallable, Category = "Build")
	void OnItemSelected(TSubclassOf<APlaceableActor> PlaceableClass);

protected:
	virtual void NativeConstruct() override;

	/** Container for dynamically-created item entries. Bind in Blueprint. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, OptionalWidget), Category = "Build")
	UVerticalBox* InventoryContainer;

	/** Class of the per-item entry widget. Set in Blueprint defaults. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Build")
	TSubclassOf<UUserWidget> ItemEntryWidgetClass;
};
