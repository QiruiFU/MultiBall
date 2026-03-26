// Copyright Autonomix. All Rights Reserved.

#include "DirectChipsRuleComponent.h"
#include "BallActor.h"
#include "PlaceableActor.h"
#include "UFloatingScoreWidget.h"

UDirectChipsRuleComponent::UDirectChipsRuleComponent()
{
	ChipsAdded = 10;
}

void UDirectChipsRuleComponent::ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable)
{
	if (!Ball || !Placeable) return;

	if (ChipsAdded > 0)
	{
		Ball->AddChips(ChipsAdded);
		
		if (Placeable->WidgetInstance)
		{
			Placeable->WidgetInstance->OnScoreChanged(ChipsAdded, true);
		}

		UE_LOG(LogTemp, Log, TEXT("[Scoring] DirectChipsRule on %s: Added +%d chips to %s. New Chips: %d"),
		       *Placeable->GetName(), ChipsAdded, *Ball->GetName(), Ball->GetScoreData().Chips);
	}
}
