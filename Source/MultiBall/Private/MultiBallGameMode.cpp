// Copyright Autonomix. All Rights Reserved.

#include "MultiBallGameMode.h"
#include "MultiBallPlayerState.h"
#include "MultiBallPlayerController.h"
#include "BallEmitterActor.h"
#include "ShopComponent.h"
#include "ScoreSubsystem.h"
#include "OpponentDataAsset.h"
#include "BoardActor.h"
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

	// Start the game
	EnterShopPhase();
}

void AMultiBallGameMode::EnterShopPhase()
{
	CurrentPhase = EGamePhase::Shop;
	UE_LOG(LogTemp, Log, TEXT("GameMode: === SHOP PHASE (Round %d) ==="), CurrentRound);

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

	// Start Drop phase timeout timer
	GetWorld()->GetTimerManager().ClearTimer(DropPhaseTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(DropPhaseTimerHandle, this,
		&AMultiBallGameMode::OnAllBallsFinished, DropPhaseDuration, false);

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
		FScoreData RoundScore = ScoreSys->GetRoundScore();
		bPlayerWon = ScoreSys->DidPlayerWin(CurrentOpponent.TargetScore);

		UE_LOG(LogTemp, Log, TEXT("GameMode: Player scored %lld vs opponent target %lld — %s!"),
		       RoundScore.GetTotalScore(), CurrentOpponent.TargetScore,
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
		// Clear timer to prevent double-transition
		GetWorld()->GetTimerManager().ClearTimer(DropPhaseTimerHandle);

		UE_LOG(LogTemp, Log, TEXT("GameMode: All balls settled / timeout. Transitioning to Rewards."));
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
		ScoreSys->AddBallScore(CurrentOpponent.TargetScore + 1, 1.0f);
	}

	// Stop balls and clear timer
	if (BallEmitter)
	{
		BallEmitter->StopDropSequence();
	}
	GetWorld()->GetTimerManager().ClearTimer(DropPhaseTimerHandle);

	EnterRewardsPhase();
}
