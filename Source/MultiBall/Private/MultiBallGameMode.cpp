// Copyright Autonomix. All Rights Reserved.

#include "MultiBallGameMode.h"
#include "MultiBallPlayerState.h"
#include "MultiBallPlayerController.h"
#include "BallEmitterActor.h"
#include "ShopComponent.h"
#include "ScoreSubsystem.h"
#include "OpponentDataAsset.h"
#include "BoardActor.h"
#include "BoardLayoutDataAsset.h"
#include "PlaceableActor.h"
#include "FlipperActor.h"
#include "BoardCameraPawn.h"
#include "SpecialSkillSubsystem.h"
#include "Kismet/GameplayStatics.h"

AMultiBallGameMode::AMultiBallGameMode()
{
	PlayerStateClass = AMultiBallPlayerState::StaticClass();
	PlayerControllerClass = AMultiBallPlayerController::StaticClass();
	DefaultPawnClass = ABoardCameraPawn::StaticClass();

	ShopComponent = CreateDefaultSubobject<UShopComponent>(TEXT("ShopComponent"));

	CurrentPhase = EGamePhase::Shop;
	CurrentRound = 1;
	MaxRounds = 10;
	WinRewardCoins = 50;
	StartingCoins = 100;
	DropPhaseDuration = 15.0f;
	
	BaseTargetScore = 50;
	TargetScoreGrowthRate = 1.5f;
	MinimumScoreIncrement = 25;
}

void AMultiBallGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Give starting coins to all players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
			if (PS)
			{
				PS->PlayerCoins = StartingCoins;
			}
		}
	}

	// Auto-find BallEmitterActor in the level if not manually set
	if (!BallEmitter)
	{
		BallEmitter = Cast<ABallEmitterActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ABallEmitterActor::StaticClass()));
		if (BallEmitter)
		{
			UE_LOG(LogTemp, Log, TEXT("GameMode: Auto-found BallEmitterActor in level."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameMode: No BallEmitterActor found! Place one in the level."));
		}
	}

	// Auto-find BoardActor in the level if not manually set
	if (!BoardActor)
	{
		BoardActor = Cast<ABoardActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ABoardActor::StaticClass()));
		if (BoardActor)
		{
			UE_LOG(LogTemp, Log, TEXT("GameMode: Auto-found BoardActor in level."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameMode: No BoardActor found! Place one in the level."));
		}
	}

	// Load first opponent (or set a default if no data asset)
	if (OpponentRoster && OpponentRoster->Opponents.Num() > 0)
	{
		CurrentOpponent = OpponentRoster->Opponents[0];
	}
	else
	{
		// Default opponent for testing
		CurrentOpponent.Name = FText::FromString(TEXT("Level 1 Opponent"));
		CurrentOpponent.TargetScore = BaseTargetScore > 0 ? BaseTargetScore : 50;
		CurrentOpponent.Difficulty = 1;
		UE_LOG(LogTemp, Log, TEXT("GameMode: No OpponentRoster set. Using dynamic target score: %lld"), CurrentOpponent.TargetScore);
	}

	UE_LOG(LogTemp, Log, TEXT("GameMode: Opponent: %s (Target: %lld)"),
	       *CurrentOpponent.Name.ToString(), CurrentOpponent.TargetScore);

	// Bind emitter callback
	if (BallEmitter)
	{
		BallEmitter->OnAllBallsFinished.AddDynamic(this, &AMultiBallGameMode::OnAllBallsFinished);
	}

	// Collect and hide all editor-placed fixed actors
	CollectEditorPlacedFixedActors();

	// Start the game
	EnterShopPhase();
}

void AMultiBallGameMode::EnterShopPhase()
{
	CurrentPhase = EGamePhase::Shop;
	UE_LOG(LogTemp, Log, TEXT("GameMode: === SHOP PHASE (Round %d) ==="), CurrentRound);

	// Clear leftover balls from previous round
	if (BallEmitter)
	{
		BallEmitter->ClearAllBalls();
	}

	// Clear previous fixed layout and spawn new one (DataAsset path)
	ClearFixedLayout();
	SpawnFixedLayout(CurrentRound);

	// Activate editor-placed fixed actors for this round
	ActivateFixedActorsForRound(CurrentRound);

	if (ShopComponent)
	{
		ShopComponent->RefreshShop();
	}

	OnPhaseChanged.Broadcast(CurrentPhase);
}

void AMultiBallGameMode::EnterDropPhase()
{
	CurrentPhase = EGamePhase::Drop;
	UE_LOG(LogTemp, Log, TEXT("GameMode: === DROP PHASE ==="));

	UScoreSubsystem* ScoreSys = GetWorld()->GetSubsystem<UScoreSubsystem>();
	if (ScoreSys)
	{
		ScoreSys->ResetRoundScore();
	}

	if (BallEmitter)
	{
		// Apply ExtraBalls skill
		USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
		int32 ExtraBalls = SkillSys ? SkillSys->GetExtraBalls() : 0;
		int32 TotalBalls = BallEmitter->BallsPerRound + ExtraBalls;
		BallEmitter->StartDropSequence(TotalBalls);
	}

	OnPhaseChanged.Broadcast(CurrentPhase);
}

void AMultiBallGameMode::EnterRewardsPhase()
{
	CurrentPhase = EGamePhase::Rewards;
	UE_LOG(LogTemp, Log, TEXT("GameMode: === REWARDS PHASE ==="));

	UScoreSubsystem* ScoreSys = GetWorld()->GetSubsystem<UScoreSubsystem>();
	bool bPlayerWon = false;
	if (ScoreSys)
	{
		int64 FinalScore = ScoreSys->GetFinalCalculatedScore();
		bPlayerWon = ScoreSys->DidPlayerWin(CurrentOpponent.TargetScore);

		UE_LOG(LogTemp, Log, TEXT("GameMode: Player scored %lld vs opponent target %lld — %s!"),
		       FinalScore, CurrentOpponent.TargetScore,
		       bPlayerWon ? TEXT("WIN") : TEXT("LOSE"));
	}

	if (bPlayerWon)
	{
		// Calculate bonus coins from ExtraCoins skill
		USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
		int32 BonusCoins = SkillSys ? SkillSys->GetExtraCoins() : 0;
		int32 TotalReward = WinRewardCoins + BonusCoins;

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC)
			{
				AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
				if (PS)
				{
					PS->PlayerCoins += TotalReward;
					UE_LOG(LogTemp, Log, TEXT("GameMode: Awarded %d coins (base %d + bonus %d). Total: %d"),
					       TotalReward, WinRewardCoins, BonusCoins, PS->PlayerCoins);
				}
			}
		}
	}

	OnRoundComplete.Broadcast(CurrentRound, bPlayerWon);

	CurrentRound++;

	// Sync PlayerState round numbers
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
			if (PS)
			{
				PS->CurrentRound = CurrentRound;
			}
		}
	}

	if (!bPlayerWon || CurrentRound > MaxRounds)
	{
		CurrentPhase = EGamePhase::GameOver;
		UE_LOG(LogTemp, Log, TEXT("GameMode: === GAME OVER ==="));
		OnPhaseChanged.Broadcast(CurrentPhase);
	}
	else
	{
		if (OpponentRoster && CurrentRound <= OpponentRoster->Opponents.Num())
		{
			CurrentOpponent = OpponentRoster->Opponents[CurrentRound - 1];
		}
		else
		{
			// Dynamically increase target score for endless rounds or missing roster
			int64 CurrentTarget = CurrentOpponent.TargetScore > 0 ? CurrentOpponent.TargetScore : 50;
			float Growth = TargetScoreGrowthRate > 1.0f ? TargetScoreGrowthRate : 1.5f;
			int64 MinInc = MinimumScoreIncrement > 0 ? MinimumScoreIncrement : 25;

			CurrentOpponent.Name = FText::FromString(FString::Printf(TEXT("Level %d Opponent"), CurrentRound));
			CurrentOpponent.TargetScore = FMath::Max(static_cast<int64>(CurrentTarget * Growth), CurrentTarget + MinInc);
			CurrentOpponent.Difficulty += 1;
		}

		OnPhaseChanged.Broadcast(CurrentPhase);
		// Go to skill selection instead of directly to shop
		EnterSkillSelectPhase();
	}
}

void AMultiBallGameMode::EnterSkillSelectPhase()
{
	CurrentPhase = EGamePhase::SkillSelect;
	UE_LOG(LogTemp, Log, TEXT("GameMode: === SKILL SELECT PHASE ==="));
	OnPhaseChanged.Broadcast(CurrentPhase);
}

void AMultiBallGameMode::OnSkillSelected(ESpecialSkill ChosenSkill)
{
	USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
	if (SkillSys)
	{
		SkillSys->ActivateSkill(ChosenSkill);
	}

	UE_LOG(LogTemp, Log, TEXT("GameMode: Skill selected, moving to Shop."));
	EnterShopPhase();
}

void AMultiBallGameMode::OnAllBallsFinished()
{
	if (CurrentPhase == EGamePhase::Drop)
	{
		UE_LOG(LogTemp, Log, TEXT("GameMode: All balls settled / destroyed. Transitioning to Rewards."));
		EnterRewardsPhase();
	}
}

void AMultiBallGameMode::CheatWinRound()
{
	if (CurrentPhase != EGamePhase::Drop)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode: CheatWinRound only works during Drop phase."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("GameMode: !!! CHEAT — Instant Win !!!"));

	// Set score well above the opponent target to guarantee a win
	UScoreSubsystem* ScoreSys = GetWorld()->GetSubsystem<UScoreSubsystem>();
	if (ScoreSys)
	{
		ScoreSys->AddScore(static_cast<int32>(CurrentOpponent.TargetScore + 1), 1.0f);
	}

	// Stop balls
	if (BallEmitter)
	{
		BallEmitter->StopDropSequence();
	}

	EnterRewardsPhase();
}

void AMultiBallGameMode::SpawnFixedLayout(int32 Round)
{
	if (!BoardLayoutAsset || !BoardActor)
	{
		return;
	}

	int32 LayoutIndex = Round - 1; // Round is 1-based, array is 0-based
	if (!BoardLayoutAsset->RoundLayouts.IsValidIndex(LayoutIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("GameMode: No fixed layout defined for round %d."), Round);
		return;
	}

	const FRoundLayout& Layout = BoardLayoutAsset->RoundLayouts[LayoutIndex];
	FVector BoardOrigin = BoardActor->GetActorLocation();
	FRotator BoardRotation = BoardActor->GetActorRotation();

	for (const FFixedPlacementEntry& Entry : Layout.FixedPlacements)
	{
		if (!Entry.PlaceableClass) continue;

		FVector SpawnLocation = BoardOrigin + BoardRotation.RotateVector(Entry.RelativeLocation);
		FRotator SpawnRotation = BoardRotation + Entry.RelativeRotation;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APlaceableActor* SpawnedActor = GetWorld()->SpawnActor<APlaceableActor>(
			Entry.PlaceableClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (SpawnedActor)
		{
			SpawnedActor->bIsFixed = true;

			// Apply flip for flippers
			if (Entry.bIsFlipped)
			{
				if (AFlipperActor* Flipper = Cast<AFlipperActor>(SpawnedActor))
				{
					Flipper->SetFlipped(true);
				}
			}

			FixedPlaceables.Add(SpawnedActor);
			UE_LOG(LogTemp, Log, TEXT("GameMode: Spawned fixed %s at %s"),
			       *GetNameSafe(Entry.PlaceableClass), *SpawnLocation.ToString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GameMode: Spawned %d fixed components for round %d."),
	       FixedPlaceables.Num(), Round);
}

void AMultiBallGameMode::ClearFixedLayout()
{
	for (APlaceableActor* Actor : FixedPlaceables)
	{
		if (Actor && IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	int32 Count = FixedPlaceables.Num();
	FixedPlaceables.Empty();

	if (Count > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("GameMode: Cleared %d DataAsset fixed components."), Count);
	}
}

void AMultiBallGameMode::CollectEditorPlacedFixedActors()
{
	TArray<AActor*> AllPlaceables;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlaceableActor::StaticClass(), AllPlaceables);

	for (AActor* Actor : AllPlaceables)
	{
		APlaceableActor* Placeable = Cast<APlaceableActor>(Actor);
		if (Placeable && Placeable->FixedForRound > 0)
		{
			Placeable->bIsFixed = true;
			Placeable->SetActorHiddenInGame(true);
			Placeable->SetActorEnableCollision(false);
			Placeable->SetActorTickEnabled(false);
			EditorPlacedFixedActors.Add(Placeable);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GameMode: Collected %d editor-placed fixed actors."), EditorPlacedFixedActors.Num());
}

void AMultiBallGameMode::ActivateFixedActorsForRound(int32 Round)
{
	int32 ActivatedCount = 0;
	for (APlaceableActor* Actor : EditorPlacedFixedActors)
	{
		if (!Actor || !IsValid(Actor)) continue;

		bool bShouldBeActive = (Actor->FixedForRound == Round);
		Actor->SetActorHiddenInGame(!bShouldBeActive);
		Actor->SetActorEnableCollision(bShouldBeActive);
		Actor->SetActorTickEnabled(bShouldBeActive);

		if (bShouldBeActive)
		{
			ActivatedCount++;
		}
	}

	if (ActivatedCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("GameMode: Activated %d editor-placed fixed actors for round %d."), ActivatedCount, Round);
	}
}

void AMultiBallGameMode::DeactivateAllFixedActors()
{
	for (APlaceableActor* Actor : EditorPlacedFixedActors)
	{
		if (Actor && IsValid(Actor))
		{
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
			Actor->SetActorTickEnabled(false);
		}
	}
}
