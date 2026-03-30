// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlaceableActor.h"
#include "MultiBallTypes.h"
#include "ShopWidget.generated.h"

class UButton;
class UBorder;
class UTextBlock;
class UShopComponent;

/**
 * C++ base class for the Shop UI. Reparent WBP_ShopUI to this class
 * so that PegButton / BumperButton clicks automatically trigger purchases.
 */
UCLASS()
class MULTIBALL_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// --- Auto-binding for borders ---

	/** Shop panel borders — visibility controlled by C++ based on game phase. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ShopNameBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* CoinsBorder;

	/** Text showing the current round number. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* RoundText;

public:
	/** Called by dynamic shop buttons to purchase an item */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuyItem(TSubclassOf<APlaceableActor> ItemClass);

	/**
	 * Get the display cost for an item, applying any active Shop Discount skill.
	 * bHasDiscount is set to true when a discount is active — use this in the
	 * UI to show the original price with a strikethrough.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shop")
	int32 GetDisplayCost(int32 BaseCost, bool& bHasDiscount) const;

private:
	/** Called when the game phase changes — shows/hides shop UI. */
	UFUNCTION()
	void HandlePhaseChanged(EGamePhase NewPhase);

	/** Update the round text from the GameMode. */
	void UpdateRoundText();

	/** Locate the ShopComponent on the GameMode actor. */
	UShopComponent* FindShopComponent() const;
};

