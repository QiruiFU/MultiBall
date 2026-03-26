// Copyright Autonomix. All Rights Reserved.

#include "ShopWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "MultiBallPlayerState.h"
#include "MultiBallGameMode.h"
#include "PegActor.h"
#include "BumperActor.h"
#include "Kismet/GameplayStatics.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// UI Borders will be bound automatically

	// Bind to phase changes to show/hide shop UI in C++
	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->OnPhaseChanged.AddDynamic(this, &UShopWidget::HandlePhaseChanged);
		// Apply initial state
		HandlePhaseChanged(GM->GetCurrentPhase());
	}

	UpdateRoundText();
}

void UShopWidget::NativeDestruct()
{
	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->OnPhaseChanged.RemoveDynamic(this, &UShopWidget::HandlePhaseChanged);
	}

	Super::NativeDestruct();
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

void UShopWidget::HandlePhaseChanged(EGamePhase NewPhase)
{
	ESlateVisibility Vis = (NewPhase == EGamePhase::Shop)
		? ESlateVisibility::Visible
		: ESlateVisibility::Hidden;

	if (Border_0)       Border_0->SetVisibility(Vis);
	if (ShopNameBorder) ShopNameBorder->SetVisibility(Vis);
	if (CoinsBorder)    CoinsBorder->SetVisibility(Vis);

	UE_LOG(LogTemp, Log, TEXT("ShopWidget: Phase %d -> Shop UI %s"),
	       (int32)NewPhase, Vis == ESlateVisibility::Visible ? TEXT("SHOWN") : TEXT("HIDDEN"));

	UpdateRoundText();
}

void UShopWidget::UpdateRoundText()
{
	if (!RoundText) return;

	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		RoundText->SetText(FText::FromString(
			FString::Printf(TEXT("Round %d / %d"), GM->GetCurrentRound(), GM->MaxRounds)));
	}
}
