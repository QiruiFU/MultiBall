// Copyright Autonomix. All Rights Reserved.

#include "ShopComponent.h"
#include "MultiBallPlayerState.h"
#include "MultiBallPlayerController.h"
#include "SpecialSkillSubsystem.h"
#include "Kismet/GameplayStatics.h"

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

int32 UShopComponent::GetDiscountedCost(int32 BaseCost) const
{
	USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
	if (SkillSys)
	{
		float Discount = SkillSys->GetShopDiscount();
		if (Discount > 0.0f)
		{
			return FMath::Max(1, FMath::RoundToInt(BaseCost * (1.0f - Discount)));
		}
	}
	return BaseCost;
}

bool UShopComponent::CanAfford(AMultiBallPlayerState* PlayerState, const FShopItem& Item) const
{
	if (!PlayerState)
	{
		return false;
	}
	int32 EffectiveCost = GetDiscountedCost(Item.Cost);
	return PlayerState->PlayerCoins >= EffectiveCost;
}

bool UShopComponent::TryPurchase(AMultiBallPlayerState* PlayerState, const FShopItem& Item)
{
	if (!CanAfford(PlayerState, Item))
	{
		UE_LOG(LogTemp, Log, TEXT("ShopComponent: Cannot afford %s (cost %d, have %d)."),
		       *Item.DisplayName.ToString(), Item.Cost, PlayerState ? PlayerState->PlayerCoins : 0);
		return false;
	}

	int32 EffectiveCost = GetDiscountedCost(Item.Cost);
	PlayerState->PlayerCoins -= EffectiveCost;
	PlayerState->AddToInventory(Item.PlaceableClass);
	UE_LOG(LogTemp, Log, TEXT("ShopComponent: Purchased %s for %d coins (base %d, discount applied). Remaining: %d"),
	       *Item.DisplayName.ToString(), EffectiveCost, Item.Cost, PlayerState->PlayerCoins);

	// Immediately enter placement mode so the ghost preview follows the cursor.
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AMultiBallPlayerController* MPC = Cast<AMultiBallPlayerController>(PC);
	if (MPC)
	{
		MPC->SelectPlaceable(Item.PlaceableClass);

		// Store durability override from shop config
		if (Item.MaxDurability > 0)
		{
			MPC->SetPendingDurability(Item.PlaceableClass, Item.MaxDurability);
		}
	}

	return true;
}
