// Copyright Autonomix. All Rights Reserved.

#include "SpecialSkillSubsystem.h"

TArray<FSpecialSkillData> USpecialSkillSubsystem::GetAllSkillDefinitions()
{
	TArray<FSpecialSkillData> All;

	{
		FSpecialSkillData D;
		D.SkillType = ESpecialSkill::ExtraBalls;
		D.DisplayName = FText::FromString(TEXT("Extra Balls"));
		D.Description = FText::FromString(TEXT("Drop 3 additional balls next round."));
		All.Add(D);
	}
	{
		FSpecialSkillData D;
		D.SkillType = ESpecialSkill::SplitChance;
		D.DisplayName = FText::FromString(TEXT("Split Chance"));
		D.Description = FText::FromString(TEXT("Balls have +10% chance to split on peg hit."));
		All.Add(D);
	}
	{
		FSpecialSkillData D;
		D.SkillType = ESpecialSkill::ScoreDoubler;
		D.DisplayName = FText::FromString(TEXT("Score Doubler"));
		D.Description = FText::FromString(TEXT("Double your final round score."));
		All.Add(D);
	}
	{
		FSpecialSkillData D;
		D.SkillType = ESpecialSkill::BonusMultiplier;
		D.DisplayName = FText::FromString(TEXT("Bonus Multiplier"));
		D.Description = FText::FromString(TEXT("Each ball starts with +0.5 base multiplier."));
		All.Add(D);
	}
	{
		FSpecialSkillData D;
		D.SkillType = ESpecialSkill::ExtraCoins;
		D.DisplayName = FText::FromString(TEXT("Extra Coins"));
		D.Description = FText::FromString(TEXT("Earn +30 bonus coins per round win."));
		All.Add(D);
	}
	{
		FSpecialSkillData D;
		D.SkillType = ESpecialSkill::BiggerBalls;
		D.DisplayName = FText::FromString(TEXT("Bigger Balls"));
		D.Description = FText::FromString(TEXT("Balls grow 1.5x in size, hitting more pegs."));
		All.Add(D);
	}

	return All;
}

TArray<FSpecialSkillData> USpecialSkillSubsystem::GenerateSkillChoices()
{
	TArray<FSpecialSkillData> AllDefs = GetAllSkillDefinitions();

	// Fisher-Yates shuffle
	for (int32 i = AllDefs.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		AllDefs.Swap(i, j);
	}

	// Pick first 3
	TArray<FSpecialSkillData> Choices;
	int32 Count = FMath::Min(3, AllDefs.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		Choices.Add(AllDefs[i]);
	}

	return Choices;
}

void USpecialSkillSubsystem::ActivateSkill(ESpecialSkill Skill)
{
	ActiveSkills.Add(Skill);
	UE_LOG(LogTemp, Log, TEXT("SkillSubsystem: Activated skill %d. Total active: %d"),
	       (int32)Skill, ActiveSkills.Num());
	OnSkillActivated.Broadcast(Skill);
}

void USpecialSkillSubsystem::ResetSkills()
{
	ActiveSkills.Empty();
	UE_LOG(LogTemp, Log, TEXT("SkillSubsystem: All skills reset."));
}

int32 USpecialSkillSubsystem::CountSkill(ESpecialSkill Skill) const
{
	int32 Count = 0;
	for (ESpecialSkill S : ActiveSkills)
	{
		if (S == Skill)
		{
			Count++;
		}
	}
	return Count;
}

int32 USpecialSkillSubsystem::GetExtraBalls() const
{
	return CountSkill(ESpecialSkill::ExtraBalls) * 3;
}

float USpecialSkillSubsystem::GetSplitChanceBonus() const
{
	return CountSkill(ESpecialSkill::SplitChance) * 0.10f;
}

float USpecialSkillSubsystem::GetScoreMultiplier() const
{
	int32 Stacks = CountSkill(ESpecialSkill::ScoreDoubler);
	return FMath::Pow(2.0f, static_cast<float>(Stacks));
}

float USpecialSkillSubsystem::GetBonusMultiplier() const
{
	return CountSkill(ESpecialSkill::BonusMultiplier) * 0.5f;
}

int32 USpecialSkillSubsystem::GetExtraCoins() const
{
	return CountSkill(ESpecialSkill::ExtraCoins) * 30;
}

float USpecialSkillSubsystem::GetBallScaleMultiplier() const
{
	int32 Stacks = CountSkill(ESpecialSkill::BiggerBalls);
	return FMath::Pow(1.5f, static_cast<float>(Stacks));
}
