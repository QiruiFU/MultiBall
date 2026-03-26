// Copyright Autonomix. All Rights Reserved.

#include "ScoreSubsystem.h"
#include "SpecialSkillSubsystem.h"
#include "Engine/Engine.h"

void UScoreSubsystem::AddScore(int32 Chips, float Multiplier)
{
	int64 HitPoints = static_cast<int64>(Chips * Multiplier);
	RoundScore += HitPoints;

	UE_LOG(LogTemp, Log, TEXT("ScoreSubsystem: Hit scored %lld (%d chips x %.2f mult). Base round total: %lld"),
	       HitPoints, Chips, Multiplier, RoundScore);

	// On-screen debug display
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
			FString::Printf(TEXT("Hit: +%lld"), HitPoints));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
			FString::Printf(TEXT("Round Total (pre-skill): %lld"), RoundScore));
	}

	OnRoundScoreUpdated.Broadcast(RoundScore);
}

int64 UScoreSubsystem::GetRoundScore() const
{
	return RoundScore;
}

void UScoreSubsystem::ResetRoundScore()
{
	RoundScore = 0;
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
	return static_cast<int64>(RoundScore * SkillMult);
}
