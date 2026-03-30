// Copyright Autonomix. All Rights Reserved.

#include "DurabilityBarWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Notifications/SProgressBar.h"

TSharedRef<SWidget> UDurabilityBarWidget::RebuildWidget()
{
	ProgressBar = SNew(SProgressBar)
		.Percent(1.0f)
		.FillColorAndOpacity(FLinearColor::Green)
		.BorderPadding(FVector2D::ZeroVector);

	return SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.4f))
		.Padding(FMargin(1.0f))
		[
			SNew(SBox)
			.WidthOverride(50.0f)
			.HeightOverride(5.0f)
			[
				ProgressBar.ToSharedRef()
			]
		];
}

void UDurabilityBarWidget::SetPercent(float InPercent)
{
	if (!ProgressBar.IsValid())
	{
		return;
	}

	ProgressBar->SetPercent(FMath::Clamp(InPercent, 0.0f, 1.0f));

	// Color transition: green → yellow → red based on remaining health
	FLinearColor Color;
	if (InPercent > 0.5f)
	{
		// Green to Yellow
		float T = (InPercent - 0.5f) * 2.0f;
		Color = FMath::Lerp(FLinearColor(1.0f, 0.9f, 0.0f), FLinearColor(0.2f, 1.0f, 0.2f), T);
	}
	else
	{
		// Yellow to Red
		float T = InPercent * 2.0f;
		Color = FMath::Lerp(FLinearColor(1.0f, 0.15f, 0.1f), FLinearColor(1.0f, 0.9f, 0.0f), T);
	}

	ProgressBar->SetFillColorAndOpacity(Color);
}
