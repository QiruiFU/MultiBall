// Copyright Autonomix. All Rights Reserved.

#include "BoardLayoutBakerLibrary.h"
#include "PlaceableActor.h"
#include "EngineUtils.h"
#include "UObject/Package.h"

void UBoardLayoutBakerLibrary::BakeLevelToDataAsset(UWorld* ContextWorld, UBoardLayoutDataAsset* TargetAsset, int32 RoundIndex)
{
	if (!ContextWorld || !TargetAsset || RoundIndex < 0)
	{
		return; // Invalid parameters
	}

	// Ensure RoundLayouts array is large enough
	if (TargetAsset->RoundLayouts.Num() <= RoundIndex)
	{
		TargetAsset->RoundLayouts.SetNum(RoundIndex + 1);
	}

	FRoundLayout& Layout = TargetAsset->RoundLayouts[RoundIndex];
	
	// Clear existing fixed placements for this round
	Layout.FixedPlacements.Empty();

	// Set a default name if it's currently empty
	if (Layout.LayoutName.IsEmpty())
	{
		Layout.LayoutName = FText::FromString(FString::Printf(TEXT("Round %d"), RoundIndex + 1));
	}

	// Iterate over all APlaceableActor instances in the contextual level
	for (TActorIterator<APlaceableActor> It(ContextWorld); It; ++It)
	{
		APlaceableActor* Placeable = *It;
		if (Placeable)
		{
			FFixedPlacementEntry NewEntry;
			NewEntry.PlaceableClass = Placeable->GetClass();
			NewEntry.RelativeLocation = Placeable->GetActorLocation();
			NewEntry.RelativeRotation = Placeable->GetActorRotation();

			// Guess flip state if any scale axis is inverted
			// Designers typically invert X or Y scale to mirror a flipper
			FVector Scale = Placeable->GetActorScale3D();
			if (Scale.X < 0.0f || Scale.Y < 0.0f || Scale.Z < 0.0f)
			{
				NewEntry.bIsFlipped = true;
			}
			else
			{
				NewEntry.bIsFlipped = false;
			}

			Layout.FixedPlacements.Add(NewEntry);
		}
	}

#if WITH_EDITOR
	// Mark the data asset and its overarching package as dirty 
	// so the Editor will prompt the designer to save the asset.
	TargetAsset->MarkPackageDirty();
#endif
}
