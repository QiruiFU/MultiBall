// Copyright Autonomix. All Rights Reserved.

#include "InteractionRuleComponent.h"
#include "BallActor.h"
#include "PlaceableActor.h"

UInteractionRuleComponent::UInteractionRuleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionRuleComponent::ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable)
{
	// Base class does nothing
}
