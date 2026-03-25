// Copyright Autonomix. All Rights Reserved.

#include "ScoreSubsystem.h"
#include "SpecialSkillSubsystem.h"
#include "Engine/Engine.h"

void UScoreSubsystem::AddBallScore(int32 Chips, float Multiplier)
{
	// Apply ScoreDoubler skill
	USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
	float SkillMult = SkillSys ? SkillSys->GetScoreMultiplier() : 1.0f;

	int64 BallScore = static_cast<int64>(Chips * Multiplier * SkillMult);
	RoundScore.Chips += Chips;
	// Overall multiplier grows additively with each ball
	RoundScore.Multiplier += (Multiplier - 1.0f);
	// Apply skill multiplier to the stored multiplier as well
	// (we track it separately so GetTotalScore reflects it)

	UE_LOG(LogTemp, Log, TEXT("ScoreSubsystem: Ball scored %lld (%d chips x %.2f mult x %.1f skill). Round total: %lld"),
	       BallScore, Chips, Multiplier, SkillMult, RoundScore.GetTotalScore());

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
	return GetFinalCalculatedScore() > OpponentScore;
}

int64 UScoreSubsystem::GetFinalCalculatedScore() const
{
	// Apply ScoreDoubler skill to final calculation
	USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
	float SkillMult = SkillSys ? SkillSys->GetScoreMultiplier() : 1.0f;
	return static_cast<int64>(RoundScore.GetTotalScore() * SkillMult);
}
