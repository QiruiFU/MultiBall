// Copyright Autonomix. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "InteractionRuleComponent.h"
#include "MultiplicativeScoreRuleComponent.generated.h"

/**
 * Multiplies the ball's existing multiplier by a factor.
 */
UCLASS(ClassGroup=(Scoring), meta=(BlueprintSpawnableComponent))
class MULTIBALL_API UMultiplicativeScoreRuleComponent : public UInteractionRuleComponent
{
	GENERATED_BODY()

public:
	UMultiplicativeScoreRuleComponent();

	/** Factor by which to multiply the ball's current multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring Rule")
	float MultiplierFactor;

	virtual void ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable) override;
};
