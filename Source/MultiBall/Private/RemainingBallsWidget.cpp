// Copyright Autonomix. All Rights Reserved.

#include "RemainingBallsWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "BallEmitterActor.h"
#include "Kismet/GameplayStatics.h"

URemainingBallsWidget::URemainingBallsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedRef<SWidget> URemainingBallsWidget::RebuildWidget()
{
	return SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0.0f, 50.0f, 0.0f, 0.0f)) // Top center offset 
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.05f, 0.85f))
			.Padding(FMargin(30.0f, 15.0f))
			[
				SAssignNew(RemainingText, STextBlock)
				.Text(FText::FromString(TEXT("Balls: 0")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 36))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
		];
}

void URemainingBallsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Find the ball emitter in the level and bind to its event
	ABallEmitterActor* Emitter = Cast<ABallEmitterActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABallEmitterActor::StaticClass()));

	if (Emitter)
	{
		Emitter->OnBallsRemainingChanged.AddDynamic(this, &URemainingBallsWidget::UpdateRemainingBallsText);
		
		// Initialize the text immediately
		UpdateRemainingBallsText(Emitter->GetBallsRemaining());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RemainingBallsWidget: Could not find BallEmitterActor in the level."));
	}
}

void URemainingBallsWidget::NativeDestruct()
{
	// Clean up binding if necessary (AddDynamic usually cleans up automatically on destruction,
	// but it's good practice to be mindful if the UI is destroyed before the emitter)
	ABallEmitterActor* Emitter = Cast<ABallEmitterActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABallEmitterActor::StaticClass()));

	if (Emitter)
	{
		Emitter->OnBallsRemainingChanged.RemoveDynamic(this, &URemainingBallsWidget::UpdateRemainingBallsText);
	}

	Super::NativeDestruct();
}

void URemainingBallsWidget::UpdateRemainingBallsText(int32 BallsRemaining)
{
	if (RemainingText.IsValid())
	{
		RemainingText->SetText(FText::FromString(FString::Printf(TEXT("Balls: %d"), BallsRemaining)));
	}
}
