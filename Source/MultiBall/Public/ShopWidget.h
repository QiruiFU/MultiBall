// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlaceableActor.h"
#include "MultiBallTypes.h"
#include "ShopWidget.generated.h"

class UButton;
class UBorder;

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

	// --- BindWidget: UE auto-binds these to the Blueprint widgets of the same name ---

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PegButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BumperButton;

	/** Shop panel borders — visibility controlled by C++ based on game phase. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ShopNameBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* CoinsBorder;

private:
	UFUNCTION()
	void OnPegClicked();

	UFUNCTION()
	void OnBumperClicked();

	void BuyItem(TSubclassOf<APlaceableActor> ItemClass);

	/** Called when the game phase changes — shows/hides shop UI. */
	UFUNCTION()
	void HandlePhaseChanged(EGamePhase NewPhase);
};
