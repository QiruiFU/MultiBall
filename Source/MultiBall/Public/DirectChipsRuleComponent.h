// Copyright Autonomix. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "InteractionRuleComponent.h"
#include "DirectChipsRuleComponent.generated.h"

/**
 * Directly adds chips to the ball.
 */
UCLASS(ClassGroup=(Scoring), meta=(BlueprintSpawnableComponent))
class MULTIBALL_API UDirectChipsRuleComponent : public UInteractionRuleComponent
{
	GENERATED_BODY()

public:
	UDirectChipsRuleComponent();

	/** Amount of chips to add directly to the ball. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring Rule")
	int32 ChipsAdded;

	virtual void ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable) override;
};
