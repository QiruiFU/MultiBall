// Copyright Autonomix. All Rights Reserved.

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "BoardLayoutDataAsset.h"

class SMultiBallLayoutBakerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMultiBallLayoutBakerWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	static void RegisterTabSpawner();
	static void UnregisterTabSpawner();

private:
	static const FName TabName;

	static TSharedRef<class SDockTab> SpawnTab(const class FSpawnTabArgs& SpawnTabArgs);

	FReply OnBakeClicked();
	void OnAssetSelected(const struct FAssetData& AssetData);
	void OnRoundIndexChanged(int32 NewValue);
	void OnRoundIndexCommitted(int32 NewValue, ETextCommit::Type CommitType);

	FString GetSelectedAssetPath() const;

	UBoardLayoutDataAsset* SelectedAsset = nullptr;
	int32 SelectedRoundIndex = 0;
};

#endif
