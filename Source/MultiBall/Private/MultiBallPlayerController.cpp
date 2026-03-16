// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "PlaceableActor.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"

AMultiBallPlayerController::AMultiBallPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
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

void AMultiBallPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("PlaceItem", IE_Pressed, this, &AMultiBallPlayerController::HandlePlacementClick);
}

void AMultiBallPlayerController::SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass)
{
    SelectedPlaceableClass = PlaceableClass;
}

void AMultiBallPlayerController::HandlePlacementClick()
{
    if (SelectedPlaceableClass)
    {
        FVector WorldLocation, WorldDirection;
        if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
        {
            FHitResult HitResult;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(GetPawn());

            if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + (WorldDirection * 10000.0f), ECC_Visibility, Params))
            {
                PurchasePlaceable(SelectedPlaceableClass, HitResult.Location);
            }
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
