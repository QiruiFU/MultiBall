// Copyright Autonomix. All Rights Reserved.

#include "ShopWidget.h"
#include "Components/Button.h"
#include "MultiBallPlayerState.h"
#include "PegActor.h"
#include "BumperActor.h"
#include "Kismet/GameplayStatics.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Buttons are auto-bound via BindWidget meta — just attach click handlers
	if (PegButton)
	{
		PegButton->OnClicked.AddDynamic(this, &UShopWidget::OnPegClicked);
		UE_LOG(LogTemp, Log, TEXT("ShopWidget: PegButton bound."));
	}

	if (BumperButton)
	{
		BumperButton->OnClicked.AddDynamic(this, &UShopWidget::OnBumperClicked);
		UE_LOG(LogTemp, Log, TEXT("ShopWidget: BumperButton bound."));
	}
}

void UShopWidget::OnPegClicked()
{
	UE_LOG(LogTemp, Warning, TEXT(">>> ShopWidget: Peg clicked!"));
	BuyItem(APegActor::StaticClass());
}

void UShopWidget::OnBumperClicked()
{
	UE_LOG(LogTemp, Warning, TEXT(">>> ShopWidget: Bumper clicked!"));
	BuyItem(ABumperActor::StaticClass());
}

void UShopWidget::BuyItem(TSubclassOf<APlaceableActor> ItemClass)
{
	if (!ItemClass)
	{
		return;
	}

	// Get player state
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopWidget: No owning player!"));
		return;
	}

	AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopWidget: No player state!"));
		return;
	}

	// Check cost
	APlaceableActor* CDO = ItemClass->GetDefaultObject<APlaceableActor>();
	if (!CDO)
	{
		return;
	}

	if (PS->PlayerCoins >= CDO->Cost)
	{
		PS->PlayerCoins -= CDO->Cost;
		PS->AddToInventory(ItemClass);
		UE_LOG(LogTemp, Warning, TEXT(">>> ShopWidget: Bought %s for %d coins. Remaining: %d"),
		       *GetNameSafe(ItemClass), CDO->Cost, PS->PlayerCoins);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(">>> ShopWidget: Cannot afford %s. Cost: %d, Have: %d"),
		       *GetNameSafe(ItemClass), CDO->Cost, PS->PlayerCoins);
	}
}
