
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultiBallBlueprintLibrary.generated.h"

UCLASS()
class MULTIBALL_API UMultiBallBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "MultiBall")
	static FVector GetLocationFromHitResult(const FHitResult& HitResult);
};
