// Copyright Autonomix. All Rights Reserved.

#include "MultiBallBlueprintLibrary.h"
#include "MultiBallPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMultiBallBlueprintLibrary::PurchasePlaceable(UObject* WorldContextObject, TSubclassOf<APlaceableActor> PlaceableClass, FVector Location)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
    if (AMultiBallPlayerController* MBPC = Cast<AMultiBallPlayerController>(PC))
    {
        MBPC->PurchasePlaceable(PlaceableClass, Location);
    }
}
