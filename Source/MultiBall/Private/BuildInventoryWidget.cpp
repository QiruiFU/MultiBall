// Copyright Autonomix. All Rights Reserved.

#include "BuildInventoryWidget.h"
#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "PlaceableActor.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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

	// Clear existing
	InventoryContainer->ClearChildren();
	ButtonToClassMap.Empty();

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
	if (!PS) return;

	if (PS->Inventory.Num() == 0)
	{
		UTextBlock* EmptyLabel = NewObject<UTextBlock>(InventoryContainer);
		EmptyLabel->SetText(FText::FromString(TEXT("Inventory Empty")));
		EmptyLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
		InventoryContainer->AddChildToHorizontalBox(EmptyLabel);
		return;
	}

	for (const FInventoryEntry& Entry : PS->Inventory)
	{
		if (!Entry.PlaceableClass || Entry.Count <= 0)
		{
			continue;
		}

		const APlaceableActor* CDO = Entry.PlaceableClass->GetDefaultObject<APlaceableActor>();
		FString ItemName = CDO ? CDO->GetClass()->GetName() : TEXT("Unknown");

		UButton* ItemButton = NewObject<UButton>(InventoryContainer);
		UTextBlock* ItemLabel = NewObject<UTextBlock>(ItemButton);

		ItemLabel->SetText(FText::FromString(
			FString::Printf(TEXT("%s x%d"), *ItemName, Entry.Count)));

		ItemButton->AddChild(ItemLabel);

		// Store mapping and bind click
		ButtonToClassMap.Add(ItemButton, Entry.PlaceableClass);
		ItemButton->OnClicked.AddDynamic(this, &UBuildInventoryWidget::HandleInventoryButtonClicked);

		UHorizontalBoxSlot* BoxSlot = InventoryContainer->AddChildToHorizontalBox(ItemButton);
		if (BoxSlot)
		{
			BoxSlot->SetPadding(FMargin(4.0f));
		}
	}
}

void UBuildInventoryWidget::HandleInventoryButtonClicked()
{
	// Find which button was clicked by checking hover state
	for (auto& Pair : ButtonToClassMap)
	{
		if (Pair.Key && Pair.Key->IsHovered())
		{
			UE_LOG(LogTemp, Log, TEXT("Inventory: Selected %s"), *GetNameSafe(Pair.Value));
			OnItemSelected(Pair.Value);
			return;
		}
	}
}

void UBuildInventoryWidget::OnItemSelected(TSubclassOf<APlaceableActor> PlaceableClass)
{
	AMultiBallPlayerController* PC = Cast<AMultiBallPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->SelectPlaceable(PlaceableClass);
		UE_LOG(LogTemp, Log, TEXT("BuildInventoryWidget: Selected %s for placement."), *GetNameSafe(PlaceableClass));
	}
}
