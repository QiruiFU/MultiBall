// Copyright Autonomix. All Rights Reserved.

#include "MultiBallBlueprintLibrary.h"

FVector UMultiBallBlueprintLibrary::GetLocationFromHitResult(const FHitResult& HitResult)
{
	return HitResult.Location;
}
