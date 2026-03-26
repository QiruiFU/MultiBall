// Copyright Autonomix. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionRuleComponent.generated.h"

class ABallActor;
class APlaceableActor;

/**
 * Base abstract component for defining rules that happen when a ball hits an object.
 */
UCLASS(Abstract, ClassGroup=(Scoring), meta=(BlueprintSpawnableComponent))
class MULTIBALL_API UInteractionRuleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionRuleComponent();

	/** Apply the interaction rule to the ball. Override this in child classes. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction Rule")
	void ApplyRule(ABallActor* Ball, APlaceableActor* Placeable);
	virtual void ApplyRule_Implementation(ABallActor* Ball, APlaceableActor* Placeable);
};
