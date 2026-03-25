// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class STextBlock;
class SButton;

/**
 * Pure C++ Slate Widget displayed when the player loses the game.
 * Shows the final score, the target score, and provides a restart button.
 */
UCLASS()
class MULTIBALL_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGameOverWidget(const FObjectInitializer& ObjectInitializer);

	/** Initialize the text blocks with the scores. */
	void SetupScoreDisplay(int64 FinalScore, int64 TargetScore);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	int64 DisplayFinalScore = 0;
	int64 DisplayTargetScore = 0;

	TSharedPtr<STextBlock> ScoreText;

	FReply OnRestartClicked();
};
