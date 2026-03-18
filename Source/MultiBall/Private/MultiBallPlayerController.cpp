// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "MultiBallGameMode.h"
#include "ShopComponent.h"
#include "PlaceableActor.h"
#include "BoardActor.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "PegActor.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

AMultiBallPlayerController::AMultiBallPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    GhostPreviewActor = nullptr;
    bIsPlacementValid = false;
    CachedBoardActor = nullptr;
    BuildWidget = nullptr;
    GhostValidMaterial = nullptr;
    GhostInvalidMaterial = nullptr;

    PrimaryActorTick.bCanEverTick = true;

    // Load translucent placement materials
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ValidMat(
        TEXT("/Engine/EngineMaterials/WorldGridMaterial"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> InvalidMat(
        TEXT("/Engine/EngineMaterials/WorldGridMaterial"));

    // We'll create dynamic material instances at runtime to set color + opacity
}

void AMultiBallPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Create Shop UI
    if (IsLocalController() && ShopWidgetClass)
    {
        UUserWidget* ShopWidget = CreateWidget<UUserWidget>(this, ShopWidgetClass);
        if (ShopWidget)
        {
            ShopWidget->AddToViewport();
        }
    }

    // Bind to phase changes
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->OnPhaseChanged.AddDynamic(this, &AMultiBallPlayerController::HandlePhaseChanged);

        // Cache board actor reference
        CachedBoardActor = GM->BoardActor;
    }
}

void AMultiBallPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Left click for placement
    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AMultiBallPlayerController::HandlePlacementClick);

    // Debug key bindings: 1=Shop, 2=Build, 3=Drop
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterShop);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterBuild);
    InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterDrop);
}

void AMultiBallPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Detect Blueprint changes to SelectedPlaceableClass
    if (SelectedPlaceableClass != LastSelectedClass)
    {
        UE_LOG(LogTemp, Warning, TEXT(">>> DETECTED SelectedPlaceableClass changed to: %s (was: %s)"),
               *GetNameSafe(SelectedPlaceableClass), *GetNameSafe(LastSelectedClass));

        AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
        if (GM && GM->GetCurrentPhase() == EGamePhase::Shop && SelectedPlaceableClass)
        {
            // Auto-buy in Shop phase
            AMultiBallPlayerState* PS = GetPlayerState<AMultiBallPlayerState>();
            if (PS)
            {
                APlaceableActor* CDO = SelectedPlaceableClass->GetDefaultObject<APlaceableActor>();
                if (CDO && PS->PlayerCoins >= CDO->Cost)
                {
                    PS->PlayerCoins -= CDO->Cost;
                    PS->AddToInventory(SelectedPlaceableClass);
                    UE_LOG(LogTemp, Warning, TEXT(">>> Shop Buy (via Tick): %s for %d. Coins: %d"),
                           *GetNameSafe(SelectedPlaceableClass), CDO->Cost, PS->PlayerCoins);
                }
            }
        }
        else if (GM && GM->GetCurrentPhase() == EGamePhase::Build && SelectedPlaceableClass)
        {
            DestroyGhostPreview();
            SpawnGhostPreview();
        }

        LastSelectedClass = SelectedPlaceableClass;
    }

    UpdateGhostPreview();
}

void AMultiBallPlayerController::SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass)
{
    SelectedPlaceableClass = PlaceableClass;
    UE_LOG(LogTemp, Log, TEXT("Selected placeable: %s"), *GetNameSafe(PlaceableClass));

    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM)
    {
        return;
    }

    if (GM->GetCurrentPhase() == EGamePhase::Shop)
    {
        // --- SHOP: immediately buy (deduct coins, add to inventory) ---
        AMultiBallPlayerState* PS = GetPlayerState<AMultiBallPlayerState>();
        if (PS && PlaceableClass)
        {
            APlaceableActor* DefaultPlaceable = PlaceableClass->GetDefaultObject<APlaceableActor>();
            if (DefaultPlaceable && PS->PlayerCoins >= DefaultPlaceable->Cost)
            {
                PS->PlayerCoins -= DefaultPlaceable->Cost;
                PS->AddToInventory(PlaceableClass);
                UE_LOG(LogTemp, Warning, TEXT(">>> Shop Buy: %s for %d coins. Remaining: %d"),
                       *GetNameSafe(PlaceableClass), DefaultPlaceable->Cost, PS->PlayerCoins);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT(">>> Shop: Cannot afford %s. Cost: %d, Have: %d"),
                       *GetNameSafe(PlaceableClass),
                       DefaultPlaceable ? DefaultPlaceable->Cost : -1,
                       PS ? PS->PlayerCoins : 0);
            }
        }
    }
    else if (GM->GetCurrentPhase() == EGamePhase::Build)
    {
        // --- BUILD: update ghost preview ---
        DestroyGhostPreview();
        if (SelectedPlaceableClass)
        {
            SpawnGhostPreview();
        }
    }
}

void AMultiBallPlayerController::SetSelectedPlaceableClass(TSubclassOf<APlaceableActor> NewClass)
{
    // This is called automatically when Blueprint sets SelectedPlaceableClass
    // Redirect to SelectPlaceable which handles shop buying + ghost updates
    UE_LOG(LogTemp, Warning, TEXT(">>> SetSelectedPlaceableClass called: %s"), *GetNameSafe(NewClass));
    SelectPlaceable(NewClass);
}

bool AMultiBallPlayerController::BuyPlaceable(TSubclassOf<APlaceableActor> PlaceableClass)
{
    if (!PlaceableClass)
    {
        return false;
    }

    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    AMultiBallPlayerState* PS = GetPlayerState<AMultiBallPlayerState>();

    if (!GM || !GM->ShopComponent || !PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuyPlaceable: Missing GameMode, ShopComponent, or PlayerState."));
        return false;
    }

    // Find the matching shop item
    for (const FShopItem& Item : GM->ShopComponent->AvailableItems)
    {
        if (Item.PlaceableClass == PlaceableClass)
        {
            bool bSuccess = GM->ShopComponent->TryPurchase(PS, Item);
            if (bSuccess)
            {
                UE_LOG(LogTemp, Log, TEXT("BuyPlaceable: Bought %s. Coins: %d"), *GetNameSafe(PlaceableClass), PS->PlayerCoins);
            }
            return bSuccess;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("BuyPlaceable: %s not found in available items."), *GetNameSafe(PlaceableClass));
    return false;
}

void AMultiBallPlayerController::HandlePhaseChanged(EGamePhase NewPhase)
{
    if (NewPhase == EGamePhase::Build)
    {
        UE_LOG(LogTemp, Log, TEXT("PlayerController: Entering Build phase."));

        // Cache board actor if not already done
        if (!CachedBoardActor)
        {
            AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
            if (GM)
            {
                CachedBoardActor = GM->BoardActor;
            }
        }

        // Show build inventory widget
        if (IsLocalController() && BuildWidgetClass && !BuildWidget)
        {
            BuildWidget = CreateWidget<UUserWidget>(this, BuildWidgetClass);
            if (BuildWidget)
            {
                BuildWidget->AddToViewport();
            }
        }

        // If a placeable is selected, spawn the ghost
        if (SelectedPlaceableClass)
        {
            SpawnGhostPreview();
        }
    }
    else
    {
        // Leaving build phase — clean up
        DestroyGhostPreview();

        if (BuildWidget)
        {
            BuildWidget->RemoveFromParent();
            BuildWidget = nullptr;
        }
    }
}

void AMultiBallPlayerController::SpawnGhostPreview()
{
    // Destroy any existing ghost
    DestroyGhostPreview();

    if (!SelectedPlaceableClass)
    {
        return;
    }

    // Spawn the ghost actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    GhostPreviewActor = GetWorld()->SpawnActor<APlaceableActor>(
        SelectedPlaceableClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (!GhostPreviewActor)
    {
        return;
    }

    // Disable all collision so it doesn't interfere with gameplay
    GhostPreviewActor->SetActorEnableCollision(false);

    // Make translucent — iterate all mesh components
    TArray<UStaticMeshComponent*> MeshComponents;
    GhostPreviewActor->GetComponents<UStaticMeshComponent>(MeshComponents);
    for (UStaticMeshComponent* Mesh : MeshComponents)
    {
        if (Mesh)
        {
            // Create dynamic material instance for translucency
            int32 NumMaterials = Mesh->GetNumMaterials();
            for (int32 i = 0; i < NumMaterials; i++)
            {
                UMaterialInstanceDynamic* DynMat = Mesh->CreateAndSetMaterialInstanceDynamic(i);
                if (DynMat)
                {
                    DynMat->SetScalarParameterValue(TEXT("Opacity"), 0.5f);
                }
            }
            // Use translucent rendering
            Mesh->SetRenderCustomDepth(true);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Ghost preview spawned for %s"), *GetNameSafe(SelectedPlaceableClass));
}

void AMultiBallPlayerController::DestroyGhostPreview()
{
    if (GhostPreviewActor)
    {
        GhostPreviewActor->Destroy();
        GhostPreviewActor = nullptr;
    }
    bIsPlacementValid = false;
}

void AMultiBallPlayerController::UpdateGhostPreview()
{
    if (!GhostPreviewActor)
    {
        return;
    }

    // Raycast from mouse position to the board
    FVector WorldLocation, WorldDirection;
    if (!DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        return;
    }

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());
    Params.AddIgnoredActor(GhostPreviewActor);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        WorldLocation,
        WorldLocation + (WorldDirection * 10000.0f),
        ECC_Visibility,
        Params);

    if (bHit)
    {
        GhostPreviewActor->SetActorLocation(HitResult.Location);
        GhostPreviewActor->SetActorHiddenInGame(false);

        // Check placement validity
        bIsPlacementValid = false;
        if (CachedBoardActor && SelectedPlaceableClass)
        {
            bIsPlacementValid = CachedBoardActor->IsLocationValid(HitResult.Location, SelectedPlaceableClass);
        }

        // Update ghost color based on validity
        FLinearColor GhostColor = bIsPlacementValid
            ? FLinearColor(0.0f, 1.0f, 0.2f, 0.5f)   // Green
            : FLinearColor(1.0f, 0.1f, 0.1f, 0.5f);   // Red

        TArray<UStaticMeshComponent*> MeshComponents;
        GhostPreviewActor->GetComponents<UStaticMeshComponent>(MeshComponents);
        for (UStaticMeshComponent* Mesh : MeshComponents)
        {
            if (Mesh)
            {
                int32 NumMaterials = Mesh->GetNumMaterials();
                for (int32 i = 0; i < NumMaterials; i++)
                {
                    UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(i));
                    if (DynMat)
                    {
                        DynMat->SetVectorParameterValue(TEXT("Color"), GhostColor);
                    }
                }
            }
        }
    }
    else
    {
        // No hit — hide the ghost
        GhostPreviewActor->SetActorHiddenInGame(true);
        bIsPlacementValid = false;
    }
}

// --- HandlePlacementClick: Build phase only ---
void AMultiBallPlayerController::HandlePlacementClick()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM || GM->GetCurrentPhase() != EGamePhase::Build)
    {
        return;
    }

    if (!SelectedPlaceableClass)
    {
        UE_LOG(LogTemp, Log, TEXT("No placeable selected."));
        return;
    }

    if (!bIsPlacementValid)
    {
        UE_LOG(LogTemp, Log, TEXT("Invalid placement location."));
        return;
    }

    AMultiBallPlayerState* PS = GetPlayerState<AMultiBallPlayerState>();
    if (!PS || PS->GetInventoryCount(SelectedPlaceableClass) <= 0)
    {
        UE_LOG(LogTemp, Log, TEXT("No inventory for %s."), *GetNameSafe(SelectedPlaceableClass));
        return;
    }

    FVector PlacementLocation = GhostPreviewActor ? GhostPreviewActor->GetActorLocation() : FVector::ZeroVector;
    PurchasePlaceable(SelectedPlaceableClass, PlacementLocation);
}

void AMultiBallPlayerController::PurchasePlaceable_Implementation(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location)
{
    UE_LOG(LogTemp, Warning, TEXT(">>> PurchasePlaceable CALLED. Class: %s, HasAuthority: %s"),
           *GetNameSafe(PlaceableClass), HasAuthority() ? TEXT("YES") : TEXT("NO"));

    if (HasAuthority())
    {
        AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
        AMultiBallPlayerState* MBPlayerState = GetPlayerState<AMultiBallPlayerState>();

        UE_LOG(LogTemp, Warning, TEXT(">>> GM: %s, PlayerState: %s"),
               GM ? TEXT("OK") : TEXT("NULL"),
               MBPlayerState ? TEXT("OK") : TEXT("NULL"));

        if (!MBPlayerState || !PlaceableClass || !GM)
        {
            UE_LOG(LogTemp, Warning, TEXT(">>> EARLY RETURN: missing GM/PlayerState/Class"));
            return;
        }

        EGamePhase CurrentPhase = GM->GetCurrentPhase();
        UE_LOG(LogTemp, Warning, TEXT(">>> CurrentPhase: %d (0=Shop, 1=Build, 2=Drop). Coins BEFORE: %d"),
               (int32)CurrentPhase, MBPlayerState->PlayerCoins);

        if (CurrentPhase == EGamePhase::Shop)
        {
            // --- SHOP PHASE: deduct coins, add to inventory (no spawn) ---
            APlaceableActor* DefaultPlaceable = PlaceableClass->GetDefaultObject<APlaceableActor>();
            if (DefaultPlaceable && MBPlayerState->PlayerCoins >= DefaultPlaceable->Cost)
            {
                MBPlayerState->PlayerCoins -= DefaultPlaceable->Cost;
                MBPlayerState->AddToInventory(PlaceableClass);
                UE_LOG(LogTemp, Warning, TEXT(">>> Shop: Bought %s for %d. Coins AFTER: %d"),
                       *GetNameSafe(PlaceableClass), DefaultPlaceable->Cost, MBPlayerState->PlayerCoins);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT(">>> Shop: Cannot afford %s. Cost: %d, Have: %d"),
                       *GetNameSafe(PlaceableClass),
                       DefaultPlaceable ? DefaultPlaceable->Cost : -1,
                       MBPlayerState->PlayerCoins);
            }
        }
        else if (CurrentPhase == EGamePhase::Build)
        {
            // --- BUILD PHASE: consume from inventory, spawn actor ---
            if (MBPlayerState->GetInventoryCount(PlaceableClass) <= 0)
            {
                UE_LOG(LogTemp, Warning, TEXT(">>> Build: No inventory for %s."), *GetNameSafe(PlaceableClass));
                return;
            }

            MBPlayerState->RemoveFromInventory(PlaceableClass);

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            APlaceableActor* NewPlaceable = GetWorld()->SpawnActor<APlaceableActor>(
                PlaceableClass, Location, FRotator::ZeroRotator, SpawnParams);

            if (GM->BoardActor && NewPlaceable)
            {
                GM->BoardActor->RegisterPlaceable(NewPlaceable);
            }

            UE_LOG(LogTemp, Warning, TEXT(">>> Build: Placed %s at (%.0f, %.0f, %.0f)"),
                   *GetNameSafe(PlaceableClass), Location.X, Location.Y, Location.Z);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT(">>> Phase %d - not Shop or Build, doing nothing."), (int32)CurrentPhase);
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
