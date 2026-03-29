// Copyright Autonomix. All Rights Reserved.

#if WITH_EDITOR

#include "SMultiBallLayoutBakerWindow.h"
#include "BoardLayoutBakerLibrary.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SButton.h"
#include "Editor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "MultiBallEditor"

const FName SMultiBallLayoutBakerWindow::TabName("MultiBallLayoutBaker");

void SMultiBallLayoutBakerWindow::RegisterTabSpawner()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		TabName, 
		FOnSpawnTab::CreateStatic(&SMultiBallLayoutBakerWindow::SpawnTab))
		.SetDisplayName(LOCTEXT("LayoutBakerTabTitle", "Board Layout Baker"))
		.SetTooltipText(LOCTEXT("LayoutBakerTooltipText", "Open the Board Layout Baker tools."))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void SMultiBallLayoutBakerWindow::UnregisterTabSpawner()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);
}

TSharedRef<SDockTab> SMultiBallLayoutBakerWindow::SpawnTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SMultiBallLayoutBakerWindow)
		];
}

void SMultiBallLayoutBakerWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.Padding(10.0f)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)

			// Asset Picker row
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TargetAssetLabel", "Target Layout Asset:"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UBoardLayoutDataAsset::StaticClass())
					.ObjectPath(this, &SMultiBallLayoutBakerWindow::GetSelectedAssetPath)
					.OnObjectChanged(this, &SMultiBallLayoutBakerWindow::OnAssetSelected)
					.AllowClear(true)
					.DisplayUseSelected(true)
					.DisplayBrowse(true)
				]
			]

			// Round index row
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("RoundIndexLabel", "Round Index:"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SSpinBox<int32>)
					.Value(this->SelectedRoundIndex)
					.MinValue(0)
					.MaxValue(99)
					.OnValueChanged(this, &SMultiBallLayoutBakerWindow::OnRoundIndexChanged)
					.OnValueCommitted(this, &SMultiBallLayoutBakerWindow::OnRoundIndexCommitted)
				]
			]

			// Bake Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 15.0f)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Text(LOCTEXT("BakeButtonLabel", "BAKE CURRENT SCENE TO ASSET"))
				.OnClicked(this, &SMultiBallLayoutBakerWindow::OnBakeClicked)
				.ContentPadding(FMargin(10.0f, 10.0f))
			]
		]
	];
}

void SMultiBallLayoutBakerWindow::OnAssetSelected(const FAssetData& AssetData)
{
	SelectedAsset = Cast<UBoardLayoutDataAsset>(AssetData.GetAsset());
}

FString SMultiBallLayoutBakerWindow::GetSelectedAssetPath() const
{
	return SelectedAsset ? SelectedAsset->GetPathName() : FString();
}

void SMultiBallLayoutBakerWindow::OnRoundIndexChanged(int32 NewValue)
{
	SelectedRoundIndex = NewValue;
}

void SMultiBallLayoutBakerWindow::OnRoundIndexCommitted(int32 NewValue, ETextCommit::Type CommitType)
{
	SelectedRoundIndex = NewValue;
}

FReply SMultiBallLayoutBakerWindow::OnBakeClicked()
{
	if (!SelectedAsset)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ErrorNoAsset", "Please select a BoardLayoutDataAsset first!"));
		return FReply::Handled();
	}

	UWorld* EditorWorld = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!EditorWorld)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ErrorNoWorld", "Cannot find active editor world."));
		return FReply::Handled();
	}

	UBoardLayoutBakerLibrary::BakeLevelToDataAsset(EditorWorld, SelectedAsset, SelectedRoundIndex);
	
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("BakeSuccess", "Bake Successful! Data asset has been marked as dirty, make sure to save it."));

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif
