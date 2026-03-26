// Copyright Autonomix. All Rights Reserved.

#include "AdditiveScoreRuleComponent.h"
#include "BallActor.h"
#include "PlaceableActor.h"
#include "UFloatingScoreWidget.h"

UAdditiveScoreRuleComponent::UAdditiveScoreRuleComponent()
{
	MultiplierAdded = 0.2f;
}

void UAdditiveScoreRuleComponent::ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable)
{
	if (!Ball || !Placeable) return;

	if (MultiplierAdded > 0.0f)
	{
		Ball->AddMultiplier(MultiplierAdded);
		
		if (Placeable->WidgetInstance)
		{
			Placeable->WidgetInstance->OnScoreChanged(MultiplierAdded, false);
		}

		UE_LOG(LogTemp, Log, TEXT("[Scoring] AdditiveRule on %s: Added +%.2fx to %s. New Multiplier: %.2fx"),
		       *Placeable->GetName(), MultiplierAdded, *Ball->GetName(), Ball->GetScoreData().Multiplier);
	}
}
