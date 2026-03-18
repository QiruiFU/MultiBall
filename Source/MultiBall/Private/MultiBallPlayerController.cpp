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
    BuildWidget = nullptr;
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

    // Bind to phase changes so we can show/hide Build widget
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->OnPhaseChanged.AddDynamic(this, &AMultiBallPlayerController::HandlePhaseChanged);

        // Trigger immediately for the initial phase (game starts in Shop)
        HandlePhaseChanged(GM->GetCurrentPhase());
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
    // Only allow placement during Build phase
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM || GM->GetCurrentPhase() != EGamePhase::Build)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Only allowed to place during Build phase!"));
        }
        return;
    }

    if (!SelectedPlaceableClass)
    {
        UE_LOG(LogTemp, Log, TEXT("No placeable selected. Pick one from inventory first."));
        return;
    }

    // Check inventory
    AMultiBallPlayerState* PS = GetPlayerState<AMultiBallPlayerState>();
    if (!PS || PS->GetInventoryCount(SelectedPlaceableClass) <= 0)
    {
        UE_LOG(LogTemp, Log, TEXT("No %s in inventory."), *GetNameSafe(SelectedPlaceableClass));
        return;
    }

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

void AMultiBallPlayerController::PurchasePlaceable_Implementation(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location)
{
    if (HasAuthority())
    {
        AMultiBallPlayerState* MBPlayerState = GetPlayerState<AMultiBallPlayerState>();
        if (MBPlayerState && PlaceableClass)
        {
            // Build phase: consume from inventory (items were pre-purchased in Shop)
            if (MBPlayerState->GetInventoryCount(PlaceableClass) <= 0)
            {
                UE_LOG(LogTemp, Log, TEXT("Server: No inventory for %s."), *GetNameSafe(PlaceableClass));
                return;
            }

            MBPlayerState->RemoveFromInventory(PlaceableClass);

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            GetWorld()->SpawnActor<APlaceableActor>(PlaceableClass, Location, FRotator::ZeroRotator, SpawnParams);
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

void AMultiBallPlayerController::HandlePhaseChanged(EGamePhase NewPhase)
{
    if (NewPhase == EGamePhase::Shop || NewPhase == EGamePhase::Build)
    {
        UE_LOG(LogTemp, Log, TEXT("Phase %d - showing inventory widget."), (int32)NewPhase);

        // Create and show inventory widget if not already showing
        if (IsLocalController() && BuildWidgetClass && !BuildWidget)
        {
            BuildWidget = CreateWidget<UUserWidget>(this, BuildWidgetClass);
            if (BuildWidget)
            {
                BuildWidget->AddToViewport();
            }
        }
    }
    else
    {
        // Other phases - remove the widget
        if (BuildWidget)
        {
            BuildWidget->RemoveFromParent();
            BuildWidget = nullptr;
        }
    }
}
