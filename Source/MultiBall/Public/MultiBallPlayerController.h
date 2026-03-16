// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
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

    /** Widget class to spawn as the shop HUD. Set in Blueprint defaults. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> ShopWidgetClass;

    UFUNCTION(Server, Reliable, WithValidation)
    void PurchasePlaceable(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location);

protected:
    virtual void BeginPlay() override;
	
};
