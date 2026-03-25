// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RemainingBallsWidget.generated.h"

class STextBlock;

/**
 * Base C++ class for the widget that displays the remaining balls during the Drop phase.
 * It automatically finds the BallEmitterActor and binds to its event to update the text.
 */
UCLASS()
class MULTIBALL_API URemainingBallsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URemainingBallsWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	TSharedPtr<STextBlock> RemainingText;

	UFUNCTION()
	void UpdateRemainingBallsText(int32 BallsRemaining);
};
