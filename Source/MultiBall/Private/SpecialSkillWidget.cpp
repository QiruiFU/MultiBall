// Copyright Autonomix. All Rights Reserved.

#include "SpecialSkillWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"

TSharedRef<SWidget> USpecialSkillWidget::RebuildWidget()
{
	ButtonTexts.Empty();
	DescTexts.Empty();

	// Build 3 skill button slots
	TSharedRef<SVerticalBox> ButtonContainer = SNew(SVerticalBox);

	for (int32 i = 0; i < 3; ++i)
	{
		TSharedPtr<STextBlock> NameText;
		TSharedPtr<STextBlock> DescText;

		ButtonContainer->AddSlot()
			.Padding(8.0f)
			.AutoHeight()
			[
				SNew(SButton)
				.OnClicked_Lambda([this, i]() { return OnSkillButtonClicked(i); })
				.HAlign(HAlign_Center)
				.ContentPadding(FMargin(20.0f, 12.0f))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					[
						SAssignNew(NameText, STextBlock)
						.Text(FText::FromString(TEXT("Skill Name")))
						.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18))
						.ColorAndOpacity(FSlateColor(FLinearColor::White))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 4.0f, 0.0f, 0.0f)
					[
						SAssignNew(DescText, STextBlock)
						.Text(FText::FromString(TEXT("Description")))
						.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 13))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)))
					]
				]
			];

		ButtonTexts.Add(NameText);
		DescTexts.Add(DescText);
	}

	return SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.05f, 0.85f))
			.Padding(FMargin(40.0f, 30.0f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(0.0f, 0.0f, 0.0f, 16.0f)
				[
					SAssignNew(TitleText, STextBlock)
					.Text(FText::FromString(TEXT("Choose a Skill!")))
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 28))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.85f, 0.2f)))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					ButtonContainer
				]
			]
		];
}

void USpecialSkillWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);
}

void USpecialSkillWidget::ShowChoices(const TArray<FSpecialSkillData>& Choices)
{
	CurrentChoices = Choices;

	for (int32 i = 0; i < 3; ++i)
	{
		if (i < Choices.Num())
		{
			if (ButtonTexts.IsValidIndex(i) && ButtonTexts[i].IsValid())
			{
				ButtonTexts[i]->SetText(Choices[i].DisplayName);
			}
			if (DescTexts.IsValidIndex(i) && DescTexts[i].IsValid())
			{
				DescTexts[i]->SetText(Choices[i].Description);
			}
		}
	}

	SetVisibility(ESlateVisibility::Visible);
}

FReply USpecialSkillWidget::OnSkillButtonClicked(int32 Index)
{
	if (CurrentChoices.IsValidIndex(Index))
	{
		ESpecialSkill Chosen = CurrentChoices[Index].SkillType;
		UE_LOG(LogTemp, Log, TEXT("SkillWidget: Player chose skill %d (%s)"),
		       (int32)Chosen, *CurrentChoices[Index].DisplayName.ToString());

		OnSkillChosen.Broadcast(Chosen);
		SetVisibility(ESlateVisibility::Collapsed);
	}
	return FReply::Handled();
}
