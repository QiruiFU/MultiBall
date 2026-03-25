// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerController.h"
#include "MultiBallPlayerState.h"
#include "MultiBallGameMode.h"
#include "PlaceableActor.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "PegActor.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NotificationWidget.h"
#include "PhaseButtonWidget.h"
#include "SpecialSkillWidget.h"
#include "SpecialSkillSubsystem.h"
#include "SpecialSkillSubsystem.h"
#include "BoardActor.h"
#include "RemainingBallsWidget.h"
#include "BallEmitterActor.h"

AMultiBallPlayerController::AMultiBallPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    BuildWidget = nullptr;
    GhostPreviewActor = nullptr;
    NotificationWidgetInstance = nullptr;
    PhaseButtonWidgetInstance = nullptr;
    SpecialSkillWidgetInstance = nullptr;
    RemainingBallsWidgetInstance = nullptr;
    PrimaryActorTick.bCanEverTick = true;
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
    }

    // Create notification widget (stays in viewport, hidden until needed)
    if (IsLocalController() && NotificationWidgetClass)
    {
        NotificationWidgetInstance = CreateWidget<UNotificationWidget>(this, NotificationWidgetClass);
        if (NotificationWidgetInstance)
        {
            NotificationWidgetInstance->AddToViewport(100);
        }
    }

    // Create phase transition buttons
    if (IsLocalController() && PhaseButtonWidgetClass)
    {
        PhaseButtonWidgetInstance = CreateWidget<UPhaseButtonWidget>(this, PhaseButtonWidgetClass);
        if (PhaseButtonWidgetInstance)
        {
            PhaseButtonWidgetInstance->AddToViewport(50);
        }
    }
    if (IsLocalController())
    {
        /*
        TSubclassOf<UUserWidget> YourClass = StaticLoadClass(UUserWidget::StaticClass(), nullptr, TEXT("/Game/UI/WBP_FloatingScoreHolder.WBP_FloatingScoreHolder_C"));
        if (YourClass != nullptr)
        {
            FloatingScoreWidgetInstance = CreateWidget<UUserWidget>(this, YourClass);
            if (FloatingScoreWidgetInstance)
            {
                FloatingScoreWidgetInstance->AddToViewport(50);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("WidgetBlueprintNotFound"));
        }
        */
    }

    // Create special skill selection widget
    if (IsLocalController())
    {
        SpecialSkillWidgetInstance = CreateWidget<USpecialSkillWidget>(this, USpecialSkillWidget::StaticClass());
        if (SpecialSkillWidgetInstance)
        {
            SpecialSkillWidgetInstance->AddToViewport(200);
            SpecialSkillWidgetInstance->OnSkillChosen.AddDynamic(this, &AMultiBallPlayerController::OnSkillSelectedFromUI);
        }
    }

    // Trigger initial phase AFTER all widgets are created
    if (GM)
    {
        HandlePhaseChanged(GM->GetCurrentPhase());
    }
}

void AMultiBallPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AMultiBallPlayerController::HandlePlacementClick);

    // Debug key bindings: 1=Shop, 2=Drop, P=Cheat Win
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterShop);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterDrop);
    InputComponent->BindKey(EKeys::P, IE_Pressed, this, &AMultiBallPlayerController::DebugCheatWin);
}

void AMultiBallPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateGhostPreview();
}

void AMultiBallPlayerController::SelectPlaceable(TSubclassOf<APlaceableActor> PlaceableClass)
{
    SelectedPlaceableClass = PlaceableClass;
    UE_LOG(LogTemp, Log, TEXT("Selected placeable: %s"), *GetNameSafe(PlaceableClass));

    // Refresh ghost if in Shop phase
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM && GM->GetCurrentPhase() == EGamePhase::Shop)
    {
        DestroyGhostPreview();
        if (SelectedPlaceableClass)
        {
            SpawnGhostPreview();
        }
    }
    else if (GM && PlaceableClass)
    {
        ShowNotification(TEXT("Items can only be placed during Shop phase!"));
    }
}

void AMultiBallPlayerController::HandlePlacementClick()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    if (GM->GetCurrentPhase() == EGamePhase::Drop)
    {
        if (GM->BallEmitter)
        {
            GM->BallEmitter->ManualDropBall();
        }
        return;
    }

    // Only allow placement during Shop phase
    if (GM->GetCurrentPhase() != EGamePhase::Shop)
    {
        ShowNotification(TEXT("Only allowed to place during Shop phase!"));
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
            if (Cast<ABoardActor>(HitResult.GetActor()))
            {
                PurchasePlaceable(SelectedPlaceableClass, HitResult.Location, HitResult.Normal * 30);

                // If inventory is now empty for this class, remove ghost
                if (PS->GetInventoryCount(SelectedPlaceableClass) <= 0)
                {
                    DestroyGhostPreview();
                    SelectedPlaceableClass = nullptr;
                }
            }
            
        }
    }
}

void AMultiBallPlayerController::PurchasePlaceable_Implementation(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location, FVector Offset)
{
    if (HasAuthority())
    {
        AMultiBallPlayerState* MBPlayerState = GetPlayerState<AMultiBallPlayerState>();
        if (MBPlayerState && PlaceableClass)
        {
            // Shop phase: consume from inventory
            if (MBPlayerState->GetInventoryCount(PlaceableClass) <= 0)
            {
                UE_LOG(LogTemp, Log, TEXT("Server: No inventory for %s."), *GetNameSafe(PlaceableClass));
                return;
            }

            MBPlayerState->RemoveFromInventory(PlaceableClass);

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;

            GetWorld()->SpawnActor<APlaceableActor>(PlaceableClass, Location + Offset, FRotationMatrix::MakeFromZ(Offset.GetSafeNormal()).Rotator(), SpawnParams);
        }
    }
}

bool AMultiBallPlayerController::PurchasePlaceable_Validate(TSubclassOf<APlaceableActor> PlaceableClass, FVector Location, FVector Offset)
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


void AMultiBallPlayerController::DebugEnterDrop()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->EnterDropPhase();
    }
}

void AMultiBallPlayerController::DebugCheatWin()
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM && GM->GetCurrentPhase() == EGamePhase::Drop)
    {
        ShowNotification(TEXT("!!! CHEAT: INSTANT WIN !!!"), 3.0f, FLinearColor::Red);
        GM->CheatWinRound();
    }
}

void AMultiBallPlayerController::HandlePhaseChanged(EGamePhase NewPhase)
{
    // Update phase button visibility
    if (PhaseButtonWidgetInstance)
    {
        PhaseButtonWidgetInstance->SetPhase(NewPhase);
    }

    if (NewPhase == EGamePhase::Shop)
    {
        UE_LOG(LogTemp, Log, TEXT("Phase %d - showing inventory widget."), (int32)NewPhase);

        if (IsLocalController() && BuildWidgetClass && !BuildWidget)
        {
            BuildWidget = CreateWidget<UUserWidget>(this, BuildWidgetClass);
            if (BuildWidget)
            {
                BuildWidget->AddToViewport();
            }
        }

        // Spawn ghost if entering Shop with a selection
        if (SelectedPlaceableClass)
        {
            SpawnGhostPreview();
        }
    }
    else if (NewPhase == EGamePhase::SkillSelect)
    {
        // Show skill selection UI
        if (SpecialSkillWidgetInstance)
        {
            USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
            if (SkillSys)
            {
                TArray<FSpecialSkillData> Choices = SkillSys->GenerateSkillChoices();
                SpecialSkillWidgetInstance->ShowChoices(Choices);
            }
        }
    }
    else if (NewPhase == EGamePhase::Drop)
    {
        if (IsLocalController() && !RemainingBallsWidgetInstance)
        {
            RemainingBallsWidgetInstance = CreateWidget<URemainingBallsWidget>(this, URemainingBallsWidget::StaticClass());
            if (RemainingBallsWidgetInstance)
            {
                RemainingBallsWidgetInstance->AddToViewport();
            }
        }
    }
    else
    {
        // Leaving active phases - clean up
        DestroyGhostPreview();

        if (BuildWidget)
        {
            BuildWidget->RemoveFromParent();
            BuildWidget = nullptr;
        }

        if (RemainingBallsWidgetInstance)
        {
            RemainingBallsWidgetInstance->RemoveFromParent();
            RemainingBallsWidgetInstance = nullptr;
        }
    }
}

// --- Ghost Preview ---

void AMultiBallPlayerController::SpawnGhostPreview()
{
    DestroyGhostPreview();

    if (!SelectedPlaceableClass) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    GhostPreviewActor = GetWorld()->SpawnActor<APlaceableActor>(
        SelectedPlaceableClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (!GhostPreviewActor) return;

    // Disable collision so it doesn't interfere
    GhostPreviewActor->SetActorEnableCollision(false);
    GhostPreviewActor->SetActorHiddenInGame(true); // hidden until first mouse hit

    // Make translucent
    TArray<UStaticMeshComponent*> Meshes;
    GhostPreviewActor->GetComponents<UStaticMeshComponent>(Meshes);
    for (UStaticMeshComponent* Mesh : Meshes)
    {
        if (Mesh)
        {
            for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
            {
                UMaterialInstanceDynamic* DynMat = Mesh->CreateAndSetMaterialInstanceDynamic(i);
                if (DynMat)
                {
                    DynMat->SetScalarParameterValue(TEXT("Opacity"), 0.4f);
                }
            }
            Mesh->SetRenderCustomDepth(true);
        }
    }
}

void AMultiBallPlayerController::DestroyGhostPreview()
{
    if (GhostPreviewActor)
    {
        GhostPreviewActor->Destroy();
        GhostPreviewActor = nullptr;
    }
}

void AMultiBallPlayerController::UpdateGhostPreview()
{
    if (!GhostPreviewActor) return;

    FVector WorldLocation, WorldDirection;
    if (!DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) return;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());
    Params.AddIgnoredActor(GhostPreviewActor);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, WorldLocation, WorldLocation + (WorldDirection * 10000.0f),
        ECC_Visibility, Params);

    if (bHit && Cast<ABoardActor>(HitResult.GetActor()))
    {
        GhostPreviewActor->SetActorLocation(HitResult.Location);
        GhostPreviewActor->SetActorRotation(FRotationMatrix::MakeFromZ(HitResult.Normal).Rotator());
        GhostPreviewActor->SetActorLocation(GhostPreviewActor->GetActorLocation() + GhostPreviewActor->GetActorUpVector() * 30);
        GhostPreviewActor->SetActorHiddenInGame(false);
    }
    else
    {
        GhostPreviewActor->SetActorHiddenInGame(true);
    }
}

void AMultiBallPlayerController::ShowNotification(const FString& Message, float Duration, FLinearColor Color)
{
    if (NotificationWidgetInstance)
    {
        NotificationWidgetInstance->ShowMessage(Message, Duration, Color);
    }
}

void AMultiBallPlayerController::OnSkillSelectedFromUI(ESpecialSkill ChosenSkill)
{
    AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->OnSkillSelected(ChosenSkill);
    }
}
