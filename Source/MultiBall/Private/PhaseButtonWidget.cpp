// Copyright Autonomix. All Rights Reserved.

#include "PhaseButtonWidget.h"
#include "Components/Button.h"
#include "MultiBallGameMode.h"
#include "Kismet/GameplayStatics.h"

void UPhaseButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartDropButton)
	{
		StartDropButton->OnClicked.AddDynamic(this, &UPhaseButtonWidget::OnStartDropClicked);
	}
	if (EnterShopButton)
	{
		EnterShopButton->OnClicked.AddDynamic(this, &UPhaseButtonWidget::OnEnterShopClicked);
	}

	// Hide all initially
	if (StartDropButton) StartDropButton->SetVisibility(ESlateVisibility::Collapsed);
	if (EnterShopButton) EnterShopButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UPhaseButtonWidget::SetPhase(EGamePhase NewPhase)
{
	if (StartDropButton)
	{
		StartDropButton->SetVisibility(
			NewPhase == EGamePhase::Shop ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (EnterShopButton)
	{
		EnterShopButton->SetVisibility(
			NewPhase == EGamePhase::Rewards ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UPhaseButtonWidget::OnStartDropClicked()
{
	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->EnterDropPhase();
	}
}

void UPhaseButtonWidget::OnEnterShopClicked()
{
	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->EnterShopPhase();
	}
}
