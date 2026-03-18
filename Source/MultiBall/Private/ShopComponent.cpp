// Copyright Autonomix. All Rights Reserved.

#include "ShopComponent.h"
#include "MultiBallPlayerState.h"

UShopComponent::UShopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ItemsPerRefresh = 4;
}

void UShopComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UShopComponent::RefreshShop()
{
	AvailableItems.Empty();

	if (AllItems.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopComponent: No items configured in AllItems!"));
		return;
	}

	// Shuffle and pick ItemsPerRefresh items
	TArray<FShopItem> Shuffled = AllItems;
	const int32 Seed = FMath::Rand();
	FRandomStream Stream(Seed);

	// Fisher-Yates shuffle
	for (int32 i = Shuffled.Num() - 1; i > 0; --i)
	{
		int32 j = Stream.RandRange(0, i);
		Shuffled.Swap(i, j);
	}

	int32 Count = FMath::Min(ItemsPerRefresh, Shuffled.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		AvailableItems.Add(Shuffled[i]);
	}

	UE_LOG(LogTemp, Log, TEXT("ShopComponent: Refreshed with %d items."), AvailableItems.Num());
	OnShopRefreshed.Broadcast();
}

bool UShopComponent::CanAfford(AMultiBallPlayerState* PlayerState, const FShopItem& Item) const
{
	if (!PlayerState)
	{
		return false;
	}
	return PlayerState->PlayerCoins >= Item.Cost;
}

bool UShopComponent::TryPurchase(AMultiBallPlayerState* PlayerState, const FShopItem& Item)
{
	if (!CanAfford(PlayerState, Item))
	{
		UE_LOG(LogTemp, Log, TEXT("ShopComponent: Cannot afford %s (cost %d, have %d)."),
		       *Item.DisplayName.ToString(), Item.Cost, PlayerState ? PlayerState->PlayerCoins : 0);
		return false;
	}

	PlayerState->PlayerCoins -= Item.Cost;
	PlayerState->AddToInventory(Item.PlaceableClass);
	UE_LOG(LogTemp, Log, TEXT("ShopComponent: Purchased %s for %d coins. Remaining: %d"),
	       *Item.DisplayName.ToString(), Item.Cost, PlayerState->PlayerCoins);
	return true;
}
