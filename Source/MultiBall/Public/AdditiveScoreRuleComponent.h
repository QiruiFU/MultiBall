// Copyright Autonomix. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "InteractionRuleComponent.h"
#include "AdditiveScoreRuleComponent.generated.h"

/**
 * Adds a fixed value to the ball's score multiplier.
 */
UCLASS(ClassGroup=(Scoring), meta=(BlueprintSpawnableComponent))
class MULTIBALL_API UAdditiveScoreRuleComponent : public UInteractionRuleComponent
{
	GENERATED_BODY()

public:
	UAdditiveScoreRuleComponent();

	/** Amount to add to the ball's current multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring Rule")
	float MultiplierAdded;

	virtual void ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable) override;
};
