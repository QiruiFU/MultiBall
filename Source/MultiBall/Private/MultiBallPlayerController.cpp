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
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/OverlapResult.h"
#include "NotificationWidget.h"
#include "PhaseButtonWidget.h"
#include "SpecialSkillWidget.h"
#include "SpecialSkillSubsystem.h"
#include "SpecialSkillSubsystem.h"
#include "BoardActor.h"
#include "RemainingBallsWidget.h"
#include "GameOverWidget.h"
#include "ScoreSubsystem.h"
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
    GameOverWidgetInstance = nullptr;
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

    // Create remaining balls widget permanently
    if (IsLocalController() && !RemainingBallsWidgetInstance)
    {
        RemainingBallsWidgetInstance = CreateWidget<URemainingBallsWidget>(this, URemainingBallsWidget::StaticClass());
        if (RemainingBallsWidgetInstance)
        {
            RemainingBallsWidgetInstance->AddToViewport();
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

    InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AMultiBallPlayerController::HandleSpacebarPressed);
    InputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &AMultiBallPlayerController::HandleSpacebarReleased);

    // Debug key bindings: 1=Shop, 2=Drop, P=Cheat Win
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterShop);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AMultiBallPlayerController::DebugEnterDrop);
    InputComponent->BindKey(EKeys::P, IE_Pressed, this, &AMultiBallPlayerController::DebugCheatWin);
}

void AMultiBallPlayerController::HandleSpacebarPressed()
{
    OnSpacebarAction.Broadcast(true);
}

void AMultiBallPlayerController::HandleSpacebarReleased()
{
    OnSpacebarAction.Broadcast(false);
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
        TArray<FHitResult> HitResults;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetPawn());
        if (GhostPreviewActor)
        {
            Params.AddIgnoredActor(GhostPreviewActor);
        }

        if (GetWorld()->LineTraceMultiByChannel(HitResults, WorldLocation, WorldLocation + (WorldDirection * 10000.0f), ECC_Visibility, Params))
        {
            const FHitResult* BoardHit = nullptr;
            for (const FHitResult& Hit : HitResults)
            {
                if (Cast<ABoardActor>(Hit.GetActor()))
                {
                    BoardHit = &Hit;
                    break;
                }
            }

            if (BoardHit)
            {
                FVector Offset = BoardHit->Normal * 30.0f;
                FVector TargetLocation = BoardHit->Location + Offset;

                if (!IsPlacementValid(TargetLocation))
                {
                    ShowNotification(TEXT("Too close to another item!"), 1.0f, FLinearColor::Red);
                    return;
                }

                PurchasePlaceable(SelectedPlaceableClass, BoardHit->Location, Offset);

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
        // RemainingBallsWidget is now initialized in BeginPlay
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
    }

    if (NewPhase == EGamePhase::GameOver)
    {
        if (IsLocalController() && !GameOverWidgetInstance)
        {
            GameOverWidgetInstance = CreateWidget<UGameOverWidget>(this, UGameOverWidget::StaticClass());
            if (GameOverWidgetInstance)
            {
                UScoreSubsystem* ScoreSys = GetWorld()->GetSubsystem<UScoreSubsystem>();
                AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(GetWorld()->GetAuthGameMode());
                if (ScoreSys && GM)
                {
                    GameOverWidgetInstance->SetupScoreDisplay(ScoreSys->GetFinalCalculatedScore(), GM->GetCurrentOpponentTargetScore());
                }
                GameOverWidgetInstance->AddToViewport(300);
            }
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

    // Collect meshes and override materials with a known engine material
    GhostMaterials.Empty();
    UMaterialInterface* BaseMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    
    TArray<UStaticMeshComponent*> Meshes;
    GhostPreviewActor->GetComponents<UStaticMeshComponent>(Meshes);
    for (UStaticMeshComponent* Mesh : Meshes)
    {
        if (Mesh && BaseMat)
        {
            for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
            {
                UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, Mesh);
                if (DynMat)
                {
                    Mesh->SetMaterial(i, DynMat);
                    GhostMaterials.Add(DynMat);
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
    GhostMaterials.Empty();
}

void AMultiBallPlayerController::UpdateGhostPreview()
{
    if (!GhostPreviewActor) return;

    FVector WorldLocation, WorldDirection;
    if (!DeprojectMousePositionToWorld(WorldLocation, WorldDirection)) return;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());
    Params.AddIgnoredActor(GhostPreviewActor);

    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->LineTraceMultiByChannel(
        HitResults, WorldLocation, WorldLocation + (WorldDirection * 10000.0f),
        ECC_Visibility, Params);

    const FHitResult* BoardHit = nullptr;
    for (const FHitResult& Hit : HitResults)
    {
        if (Cast<ABoardActor>(Hit.GetActor()))
        {
            BoardHit = &Hit;
            break;
        }
    }

    if (BoardHit)
    {
        GhostPreviewActor->SetActorLocation(BoardHit->Location);
        GhostPreviewActor->SetActorRotation(FRotationMatrix::MakeFromZ(BoardHit->Normal).Rotator());
        FVector SnapLocation = GhostPreviewActor->GetActorLocation() + GhostPreviewActor->GetActorUpVector() * 30;
        GhostPreviewActor->SetActorLocation(SnapLocation);
        GhostPreviewActor->SetActorHiddenInGame(false);

        // Update colors based on validity with pulsing effect
        bool bValid = IsPlacementValid(SnapLocation);
        FLinearColor TargetColor = bValid ? FLinearColor(0.0f, 1.0f, 0.2f) : FLinearColor(1.0f, 0.1f, 0.1f);
        
        float TimeSec = GetWorld()->GetTimeSeconds();
        float PulseAlpha = 0.3f + 0.3f * FMath::Sin(TimeSec * 8.0f); // Pulses rapidly
        
        for (UMaterialInstanceDynamic* MID : GhostMaterials)
        {
            if (MID)
            {
                MID->SetVectorParameterValue(TEXT("BaseColor"), TargetColor);
                MID->SetVectorParameterValue(TEXT("Color"), TargetColor);
                MID->SetVectorParameterValue(TEXT("EmissiveColor"), TargetColor * PulseAlpha * 2.5f);
                MID->SetScalarParameterValue(TEXT("Opacity"), FMath::Clamp(PulseAlpha + 0.1f, 0.2f, 0.8f));
            }
        }
    }
    else
    {
        GhostPreviewActor->SetActorHiddenInGame(true);
    }
}

bool AMultiBallPlayerController::IsPlacementValid(const FVector& Location) const
{
    if (!GhostPreviewActor || !GhostPreviewActor->PlacementBlockingRadius)
    {
        return true;
    }

    float Radius = GhostPreviewActor->PlacementBlockingRadius->GetScaledSphereRadius();
    FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius * 0.9f); // Slightly smaller to be forgiving

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());
    Params.AddIgnoredActor(GhostPreviewActor);

    TArray<FOverlapResult> Overlaps;
    bool bHit = GetWorld()->OverlapMultiByObjectType(
        Overlaps,
        Location,
        FQuat::Identity,
        FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects),
        Sphere,
        Params);

    if (bHit)
    {
        for (const FOverlapResult& Overlap : Overlaps)
        {
            if (AActor* HitActor = Overlap.GetActor())
            {
                if (Cast<APlaceableActor>(HitActor))
                {
                    return false; // Cannot place overlapping another placeable item
                }
            }
        }
    }
    return true;
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
