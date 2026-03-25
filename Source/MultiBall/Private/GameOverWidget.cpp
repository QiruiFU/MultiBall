// Copyright Autonomix. All Rights Reserved.

#include "GameOverWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Kismet/GameplayStatics.h"

UGameOverWidget::UGameOverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedRef<SWidget> UGameOverWidget::RebuildWidget()
{
	return SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.01f, 0.01f, 0.02f, 0.95f))
			.Padding(FMargin(50.0f, 40.0f))
			[
				SNew(SVerticalBox)
				
				// Title
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(FMargin(0, 0, 0, 20))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("GAME OVER")))
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 64))
					.ColorAndOpacity(FSlateColor(FLinearColor::Red))
				]

				// Score Text
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(FMargin(0, 0, 0, 40))
				[
					SAssignNew(ScoreText, STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("Your Score: %lld / Target: %lld"), DisplayFinalScore, DisplayTargetScore)))
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 32))
					.ColorAndOpacity(FSlateColor(FLinearColor::White))
				]

				// Restart Button
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
					.ContentPadding(FMargin(30.0f, 15.0f))
					.ButtonColorAndOpacity(FSlateColor(FLinearColor(0.1f, 0.5f, 0.1f, 1.0f)))
					.OnClicked_Lambda([this]() { return OnRestartClicked(); })
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Play Again")))
						.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
					]
				]
			]
		];
}

void UGameOverWidget::SetupScoreDisplay(int64 FinalScore, int64 TargetScore)
{
	DisplayFinalScore = FinalScore;
	DisplayTargetScore = TargetScore;

	if (ScoreText.IsValid())
	{
		FString TextStr = FString::Printf(TEXT("Your Score: %lld / Target: %lld"), DisplayFinalScore, DisplayTargetScore);
		ScoreText->SetText(FText::FromString(TextStr));
	}
}

FReply UGameOverWidget::OnRestartClicked()
{
	// Reload the current level to restart the game
	FName CurrentLevelName = *GetWorld()->GetName();
	UGameplayStatics::OpenLevel(this, CurrentLevelName, false);
	
	return FReply::Handled();
}
