// Copyright Autonomix. All Rights Reserved.

#include "MultiplicativeScoreRuleComponent.h"
#include "BallActor.h"
#include "PlaceableActor.h"
#include "UFloatingScoreWidget.h"

UMultiplicativeScoreRuleComponent::UMultiplicativeScoreRuleComponent()
{
	MultiplierFactor = 2.0f;
}

void UMultiplicativeScoreRuleComponent::ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable)
{
	if (!Ball || !Placeable) return;

	if (MultiplierFactor != 1.0f)
	{
		Ball->MultiplyMultiplier(MultiplierFactor);
		
		if (Placeable->WidgetInstance)
		{
			// Note: Assuming OnScoreChanged handles displaying this value correctly for now
			Placeable->WidgetInstance->OnScoreChanged(MultiplierFactor, false);
		}

		UE_LOG(LogTemp, Log, TEXT("[Scoring] MultiplicativeRule on %s: Multiplied %s by x%.2f. New Multiplier: %.2fx"),
		       *Placeable->GetName(), *Ball->GetName(), MultiplierFactor, Ball->GetScoreData().Multiplier);
	}
}
