// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlaceableActor.h"
#include "MultiBallBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MULTIBALL_API UMultiBallBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "MultiBall")
    static void PurchasePlaceable(UObject* WorldContextObject, TSubclassOf<APlaceableActor> PlaceableClass, FVector Location);
	
};
