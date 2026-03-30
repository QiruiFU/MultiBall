// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DurabilityBarWidget.generated.h"

class SProgressBar;

/**
 * Pure C++ health bar widget for PlaceableActor durability.
 * Renders a semi-transparent progress bar that transitions green → yellow → red.
 */
UCLASS()
class MULTIBALL_API UDurabilityBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Update the bar fill (0.0 = empty, 1.0 = full). */
	void SetPercent(float InPercent);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<SProgressBar> ProgressBar;
};
