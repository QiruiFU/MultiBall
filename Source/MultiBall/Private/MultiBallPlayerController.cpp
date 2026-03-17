// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "MultiBallGameMode.h"
#include "PlaceableActor.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "PegActor.h"
#include "GameFramework/Pawn.h"

AMultiBallPlayerController::AMultiBallPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
}

void AMultiBallPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("Adding UI"));
    if (IsLocalController() && ShopWidgetClass)
    {
        UUserWidget* ShopWidget = CreateWidget<UUserWidget>(this, ShopWidgetClass);
        if (ShopWidget)
        {
            UE_LOG(LogTemp, Log, TEXT("Added"));
            ShopWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Not Added"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Not Added"));
    }
}

void AMultiBallPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("PlaceItem", IE_Pressed, this, &AMultiBallPlayerController::HandlePlacementClick);

    // Debug key bindings: 1=Shop, 2=Build, 3=Drop
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterShop);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterBuild);
    InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterDrop);
}

void AMultiBallPlayerController::SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass)
{
    SelectedPlaceableClass = PlaceableClass;
    //Log the name of the selected placeable
	UE_LOG(LogTemp, Log, TEXT("Selected placeable: %s"), *GetNameSafe(PlaceableClass));
}

void AMultiBallPlayerController::HandlePlacementClick()
{
    UE_LOG(LogTemp, Log, TEXT("Trying to Place"));
    SelectedPlaceableClass = APegActor::StaticClass();
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
    UE_LOG(LogTemp, Log, TEXT("This is called"));
    if (HasAuthority())
    {
        UE_LOG(LogTemp, Log, TEXT("Trying to Place 2"));
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

// --- Debug Key Bindings ---

void AMultiBallPlayerController::DebugEnterShop()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->EnterShopPhase();
    }
}

void AMultiBallPlayerController::DebugEnterBuild()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->EnterBuildPhase();
    }
}

void AMultiBallPlayerController::DebugEnterDrop()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->EnterDropPhase();
    }
}
