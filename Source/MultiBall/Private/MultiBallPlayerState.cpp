// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerState.h"
#include "PlaceableActor.h"
#include "Net/UnrealNetwork.h"
#include "MultiBallPlayerController.h"
#include "Kismet/GameplayStatics.h"

AMultiBallPlayerState::AMultiBallPlayerState()
{
	PlayerCoins = 0;
	TotalScore = 0;
	CurrentRound = 1;
}

void AMultiBallPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiBallPlayerState, PlayerCoins);
	DOREPLIFETIME(AMultiBallPlayerState, TotalScore);
	DOREPLIFETIME(AMultiBallPlayerState, CurrentRound);
	DOREPLIFETIME(AMultiBallPlayerState, Inventory);
}

void AMultiBallPlayerState::AddToInventory(TSubclassOf<APlaceableActor> PlaceableClass)
{
	if (!PlaceableClass) return;

	AMultiBallPlayerController* MPC = Cast<AMultiBallPlayerController>(GetPlayerController());
	if (!MPC)
	{
		MPC = Cast<AMultiBallPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	for (FInventoryEntry& Entry : Inventory)
	{
		if (Entry.PlaceableClass == PlaceableClass)
		{
			Entry.Count++;
			OnInventoryChanged.Broadcast();

			if (MPC && MPC->IsLocalController())
			{
				MPC->SelectPlaceable(PlaceableClass);
			}
			return;
		}
	}

	FInventoryEntry NewEntry;
	NewEntry.PlaceableClass = PlaceableClass;
	NewEntry.Count = 1;
	Inventory.Add(NewEntry);
	OnInventoryChanged.Broadcast();

	if (MPC && MPC->IsLocalController())
	{
		MPC->SelectPlaceable(PlaceableClass);
	}
}

bool AMultiBallPlayerState::RemoveFromInventory(TSubclassOf<APlaceableActor> PlaceableClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].PlaceableClass == PlaceableClass)
		{
			Inventory[i].Count--;
			if (Inventory[i].Count <= 0)
			{
				Inventory.RemoveAt(i);
			}
			OnInventoryChanged.Broadcast();
			return true;
		}
	}
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
