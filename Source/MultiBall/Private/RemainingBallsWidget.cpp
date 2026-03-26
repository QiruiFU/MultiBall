// Copyright Autonomix. All Rights Reserved.

#include "RemainingBallsWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "BallEmitterActor.h"
#include "MultiBallGameMode.h"
#include "SpecialSkillSubsystem.h"
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
				.Text_UObject(this, &URemainingBallsWidget::GetBallsText)
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 36))
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
			]
		];
}

FText URemainingBallsWidget::GetBallsText() const
{
	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return FText::FromString("Balls: ?");

	if (GM->GetCurrentPhase() == EGamePhase::Drop)
	{
		ABallEmitterActor* Emitter = GM->BallEmitter;
		int32 Count = Emitter ? Emitter->GetBallsRemaining() : 0;
		return FText::FromString(FString::Printf(TEXT("Balls: %d"), Count));
	}
	else
	{
		ABallEmitterActor* Emitter = GM->BallEmitter;
		int32 BaseBalls = Emitter ? Emitter->BallsPerRound : 5;
		
		USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
		int32 ExtraBalls = SkillSys ? SkillSys->GetExtraBalls() : 0;
		
		return FText::FromString(FString::Printf(TEXT("Balls: %d"), BaseBalls + ExtraBalls));
	}
}
