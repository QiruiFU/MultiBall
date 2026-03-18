// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerState.h"
#include "PlaceableActor.h"
#include "Net/UnrealNetwork.h"

AMultiBallPlayerState::AMultiBallPlayerState()
{
	PlayerCoins = 0;
	RoundChips = 0;
	RoundMultiplier = 1.0f;
	TotalScore = 0;
	CurrentRound = 1;
}

void AMultiBallPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiBallPlayerState, PlayerCoins);
	DOREPLIFETIME(AMultiBallPlayerState, RoundChips);
	DOREPLIFETIME(AMultiBallPlayerState, RoundMultiplier);
	DOREPLIFETIME(AMultiBallPlayerState, TotalScore);
	DOREPLIFETIME(AMultiBallPlayerState, CurrentRound);
	DOREPLIFETIME(AMultiBallPlayerState, Inventory);
}

void AMultiBallPlayerState::AddToInventory(TSubclassOf<APlaceableActor> PlaceableClass)
{
	if (!PlaceableClass)
	{
		return;
	}

	for (FInventoryEntry& Entry : Inventory)
	{
		if (Entry.PlaceableClass == PlaceableClass)
		{
			Entry.Count++;
			UE_LOG(LogTemp, Log, TEXT("Inventory: +1 %s (total: %d)"), *GetNameSafe(PlaceableClass), Entry.Count);
			OnInventoryChanged.Broadcast();
			return;
		}
	}

	// New entry
	FInventoryEntry NewEntry;
	NewEntry.PlaceableClass = PlaceableClass;
	NewEntry.Count = 1;
	Inventory.Add(NewEntry);
	UE_LOG(LogTemp, Log, TEXT("Inventory: +1 %s (new, total: 1)"), *GetNameSafe(PlaceableClass));
	OnInventoryChanged.Broadcast();
}

bool AMultiBallPlayerState::RemoveFromInventory(TSubclassOf<APlaceableActor> PlaceableClass)
{
	if (!PlaceableClass)
	{
		return false;
	}

	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].PlaceableClass == PlaceableClass)
		{
			Inventory[i].Count--;
			UE_LOG(LogTemp, Log, TEXT("Inventory: -1 %s (remaining: %d)"), *GetNameSafe(PlaceableClass), Inventory[i].Count);
			if (Inventory[i].Count <= 0)
			{
				Inventory.RemoveAt(i);
			}
			OnInventoryChanged.Broadcast();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory: Cannot remove %s - not found!"), *GetNameSafe(PlaceableClass));
	return false;
}

int32 AMultiBallPlayerState::GetInventoryCount(TSubclassOf<APlaceableActor> PlaceableClass) const
{
	for (const FInventoryEntry& Entry : Inventory)
	{
		if (Entry.PlaceableClass == PlaceableClass)
		{
			return Entry.Count;
		}
	}
	return 0;
}
