// Copyright Autonomix. All Rights Reserved.

#include "PhaseButtonWidget.h"
#include "Components/Button.h"
#include "MultiBallGameMode.h"
#include "Kismet/GameplayStatics.h"

void UPhaseButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartBuildButton)
	{
		StartBuildButton->OnClicked.AddDynamic(this, &UPhaseButtonWidget::OnStartBuildClicked);
	}
	if (StartDropButton)
	{
		StartDropButton->OnClicked.AddDynamic(this, &UPhaseButtonWidget::OnStartDropClicked);
	}
	if (EnterShopButton)
	{
		EnterShopButton->OnClicked.AddDynamic(this, &UPhaseButtonWidget::OnEnterShopClicked);
	}

	// Hide all initially
	if (StartBuildButton) StartBuildButton->SetVisibility(ESlateVisibility::Collapsed);
	if (StartDropButton) StartDropButton->SetVisibility(ESlateVisibility::Collapsed);
	if (EnterShopButton) EnterShopButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UPhaseButtonWidget::SetPhase(EGamePhase NewPhase)
{
	if (StartBuildButton)
	{
		StartBuildButton->SetVisibility(
			NewPhase == EGamePhase::Shop ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (StartDropButton)
	{
		StartDropButton->SetVisibility(
			NewPhase == EGamePhase::Build ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (EnterShopButton)
	{
		EnterShopButton->SetVisibility(
			NewPhase == EGamePhase::Rewards ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UPhaseButtonWidget::OnStartBuildClicked()
{
	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->EnterBuildPhase();
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
