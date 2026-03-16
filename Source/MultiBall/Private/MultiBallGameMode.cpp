// Copyright Autonomix. All Rights Reserved.

#include "MultiBallGameMode.h"
#include "MultiBallPlayerState.h"
#include "MultiBallPlayerController.h"
#include "BallEmitterActor.h"
#include "ShopComponent.h"
#include "ScoreSubsystem.h"
#include "OpponentDataAsset.h"

AMultiBallGameMode::AMultiBallGameMode()
{
	PlayerStateClass = AMultiBallPlayerState::StaticClass();
	PlayerControllerClass = AMultiBallPlayerController::StaticClass();

	ShopComponent = CreateDefaultSubobject<UShopComponent>(TEXT("ShopComponent"));

	CurrentPhase = EGamePhase::Shop;
	CurrentRound = 1;
	MaxRounds = 10;
	WinRewardCoins = 50;
	StartingCoins = 100;
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

	// Load first opponent
	if (OpponentRoster && OpponentRoster->Opponents.Num() > 0)
	{
		CurrentOpponent = OpponentRoster->Opponents[0];
		UE_LOG(LogTemp, Log, TEXT("GameMode: First opponent: %s (Target: %lld)"),
		       *CurrentOpponent.Name.ToString(), CurrentOpponent.TargetScore);
	}

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

void AMultiBallGameMode::EnterBuildPhase()
{
	CurrentPhase = EGamePhase::Build;
	UE_LOG(LogTemp, Log, TEXT("GameMode: === BUILD PHASE ==="));
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
		BallEmitter->StartDropSequence(BallEmitter->BallsPerRound);
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
		FScoreData RoundScore = ScoreSys->GetRoundScore();
		bPlayerWon = ScoreSys->DidPlayerWin(CurrentOpponent.TargetScore);

		UE_LOG(LogTemp, Log, TEXT("GameMode: Player scored %lld vs opponent target %lld — %s!"),
		       RoundScore.GetTotalScore(), CurrentOpponent.TargetScore,
		       bPlayerWon ? TEXT("WIN") : TEXT("LOSE"));
	}

	if (bPlayerWon)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC)
			{
				AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
				if (PS)
				{
					PS->PlayerCoins += WinRewardCoins;
					UE_LOG(LogTemp, Log, TEXT("GameMode: Awarded %d coins. Total: %d"), WinRewardCoins, PS->PlayerCoins);
				}
			}
		}
	}

	OnRoundComplete.Broadcast(CurrentRound, bPlayerWon);

	CurrentRound++;
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

		OnPhaseChanged.Broadcast(CurrentPhase);
		EnterShopPhase();
	}
}

void AMultiBallGameMode::OnAllBallsFinished()
{
	if (CurrentPhase == EGamePhase::Drop)
	{
		UE_LOG(LogTemp, Log, TEXT("GameMode: All balls settled. Transitioning to Rewards."));
		EnterRewardsPhase();
	}
}
