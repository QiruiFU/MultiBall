// Copyright Autonomix. All Rights Reserved.

#include "BuildInventoryWidget.h"
#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "PlaceableActor.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

void UBuildInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind to inventory changes for auto-refresh
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
		if (PS)
		{
			PS->OnInventoryChanged.AddDynamic(this, &UBuildInventoryWidget::RefreshInventory);
		}
	}

	RefreshInventory();
}

void UBuildInventoryWidget::RefreshInventory()
{
	if (!InventoryContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildInventoryWidget: No InventoryContainer bound!"));
		return;
	}

	// Clear existing entries
	InventoryContainer->ClearChildren();

	// Get player state
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
	if (!PS)
	{
		return;
	}

	// Create a button for each inventory entry
	for (const FInventoryEntry& Entry : PS->Inventory)
	{
		if (!Entry.PlaceableClass || Entry.Count <= 0)
		{
			continue;
		}

		// Get the display name from the CDO
		const APlaceableActor* CDO = Entry.PlaceableClass->GetDefaultObject<APlaceableActor>();
		FString ItemName = CDO ? CDO->GetClass()->GetName() : TEXT("Unknown");

		// Create a button with a text label
		UButton* ItemButton = NewObject<UButton>(InventoryContainer);
		UTextBlock* ItemLabel = NewObject<UTextBlock>(ItemButton);

		ItemLabel->SetText(FText::FromString(
			FString::Printf(TEXT("%s x%d"), *ItemName, Entry.Count)));

		ItemButton->AddChild(ItemLabel);

		// Capture the class for the lambda
		TSubclassOf<APlaceableActor> CapturedClass = Entry.PlaceableClass;
		ItemButton->OnClicked.AddDynamic(this, &UBuildInventoryWidget::RefreshInventory); // Placeholder

		UVerticalBoxSlot* BoxSlot = InventoryContainer->AddChildToVerticalBox(ItemButton);
		if (BoxSlot)
		{
			BoxSlot->SetPadding(FMargin(4.0f));
		}
	}
}

void UBuildInventoryWidget::OnItemSelected(TSubclassOf<APlaceableActor> PlaceableClass)
{
	AMultiBallPlayerController* PC = Cast<AMultiBallPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->SelectPlaceable(PlaceableClass);
		UE_LOG(LogTemp, Log, TEXT("BuildInventoryWidget: Selected %s"), *GetNameSafe(PlaceableClass));
	}
}
