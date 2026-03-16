// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlaceableActor.h"
#include "MultiBallPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIBALL_API AMultiBallPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AMultiBallPlayerController();

    UFUNCTION(BlueprintCallable, Category = "Shop")
    void SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass);

    UFUNCTION(Server, Reliable, WithValidation)
    void PurchasePlaceable(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location);

    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    TSubclassOf<APlaceableActor> SelectedPlaceableClass;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    void HandlePlacementClick();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ShopWidgetClass;
	
};
