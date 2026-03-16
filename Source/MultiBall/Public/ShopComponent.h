// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MultiBallTypes.h"
#include "ShopComponent.generated.h"

class AMultiBallPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopRefreshed);

/**
 * Manages the shop inventory, purchase validation, and roguelike draft randomization.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIBALL_API UShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShopComponent();

	/** All possible items that can appear in the shop. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TArray<FShopItem> AllItems;

	/** Items currently available for purchase this round. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<FShopItem> AvailableItems;

	/** How many items to show per shop refresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 ItemsPerRefresh;

	/** Broadcast when the shop is refreshed with new items. */
	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FOnShopRefreshed OnShopRefreshed;

	/** Re-roll available items (roguelike draft). */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RefreshShop();

	/** Check if a player can afford an item. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shop")
	bool CanAfford(AMultiBallPlayerState* PlayerState, const FShopItem& Item) const;

	/** Attempt to purchase an item. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool TryPurchase(AMultiBallPlayerState* PlayerState, const FShopItem& Item);

protected:
	virtual void BeginPlay() override;
};
