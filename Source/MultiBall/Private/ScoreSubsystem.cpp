// Copyright Autonomix. All Rights Reserved.

#include "ScoreSubsystem.h"
#include "Engine/Engine.h"

void UScoreSubsystem::AddBallScore(int32 Chips, float Multiplier)
{
	int64 BallScore = static_cast<int64>(Chips * Multiplier);
	RoundScore.Chips += Chips;
	// Overall multiplier grows additively with each ball
	RoundScore.Multiplier += (Multiplier - 1.0f);

	UE_LOG(LogTemp, Log, TEXT("ScoreSubsystem: Ball scored %lld (%d chips x %.2f mult). Round total: %lld"),
	       BallScore, Chips, Multiplier, RoundScore.GetTotalScore());

	// On-screen debug display
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
			FString::Printf(TEXT("Ball: +%d chips x%.1f mult = %lld"), Chips, Multiplier, BallScore));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
			FString::Printf(TEXT("Round Total: %d chips x%.1f mult = %lld"),
				RoundScore.Chips, RoundScore.Multiplier, RoundScore.GetTotalScore()));
	}

	OnRoundScoreUpdated.Broadcast(RoundScore);
}

FScoreData UScoreSubsystem::GetRoundScore() const
{
	return RoundScore;
}

void UScoreSubsystem::ResetRoundScore()
{
	RoundScore.Reset();
	UE_LOG(LogTemp, Log, TEXT("ScoreSubsystem: Round score reset."));
}

bool UScoreSubsystem::DidPlayerWin(int64 OpponentScore) const
{
	return RoundScore.GetTotalScore() > OpponentScore;
}
