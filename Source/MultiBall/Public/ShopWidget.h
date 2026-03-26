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

private:
	/** Called when the game phase changes — shows/hides shop UI. */
	UFUNCTION()
	void HandlePhaseChanged(EGamePhase NewPhase);

	/** Update the round text from the GameMode. */
	void UpdateRoundText();
};
