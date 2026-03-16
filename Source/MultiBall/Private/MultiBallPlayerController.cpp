// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "PlaceableActor.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"


AMultiBallPlayerController::AMultiBallPlayerController()
{
}

void AMultiBallPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && ShopWidgetClass)
    {
        UUserWidget* ShopWidget = CreateWidget<UUserWidget>(this, ShopWidgetClass);
        if (ShopWidget)
        {
            ShopWidget->AddToViewport();
        }
    }
}

void AMultiBallPlayerController::PurchasePlaceable_Implementation(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location)
{
    if (HasAuthority())
    {
        AMultiBallPlayerState* MBPlayerState = GetPlayerState<AMultiBallPlayerState>();
        if (MBPlayerState && PlaceableClass)
        {
            APlaceableActor* DefaultPlaceable = PlaceableClass->GetDefaultObject<APlaceableActor>();
            if (DefaultPlaceable && MBPlayerState->PlayerCoins >= DefaultPlaceable->Cost)
            {
                MBPlayerState->PlayerCoins -= DefaultPlaceable->Cost;

                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                GetWorld()->SpawnActor<APlaceableActor>(PlaceableClass, Location, FRotator::ZeroRotator, SpawnParams);
            }
        }
    }
}

bool AMultiBallPlayerController::PurchasePlaceable_Validate(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location)
{
    return true;
}
