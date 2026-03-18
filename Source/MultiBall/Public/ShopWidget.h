// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlaceableActor.h"
#include "ShopWidget.generated.h"

class UButton;

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

	// --- BindWidget: UE auto-binds these to the Blueprint widgets of the same name ---

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* PegButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BumperButton;

private:
	UFUNCTION()
	void OnPegClicked();

	UFUNCTION()
	void OnBumperClicked();

	void BuyItem(TSubclassOf<APlaceableActor> ItemClass);
};
