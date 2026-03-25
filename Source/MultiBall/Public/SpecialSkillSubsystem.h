// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MultiBallTypes.h"
#include "SpecialSkillSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillActivated, ESpecialSkill, Skill);

/**
 * World subsystem that manages the player's acquired special skills
 * and provides helper queries for each skill's accumulated effect.
 */
UCLASS()
class MULTIBALL_API USpecialSkillSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Generate 3 random, non-duplicate skill choices. */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	TArray<FSpecialSkillData> GenerateSkillChoices();

	/** Activate a chosen skill (adds to the active list). */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ActivateSkill(ESpecialSkill Skill);

	/** Get all activated skills. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	const TArray<ESpecialSkill>& GetActiveSkills() const { return ActiveSkills; }

	/** Reset all skills (e.g. on new game). */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ResetSkills();

	// --- Accumulated Effect Queries ---

	/** Extra balls to add to the drop count. (+3 per stack) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	int32 GetExtraBalls() const;

	/** Additional split probability. (+0.10 per stack, default 0) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetSplitChanceBonus() const;

	/** Final score multiplier from ScoreDoubler. (2^stacks) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetScoreMultiplier() const;

	/** Extra base multiplier for each ball. (+0.5 per stack) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetBonusMultiplier() const;

	/** Extra coins per round win. (+30 per stack) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	int32 GetExtraCoins() const;

	/** Ball scale multiplier. (1.5^stacks) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetBallScaleMultiplier() const;

	/** Broadcast when a skill is activated. */
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillActivated OnSkillActivated;

private:
	UPROPERTY()
	TArray<ESpecialSkill> ActiveSkills;

	/** Count how many times a specific skill has been acquired. */
	int32 CountSkill(ESpecialSkill Skill) const;

	/** Master list of all skill metadata. */
	static TArray<FSpecialSkillData> GetAllSkillDefinitions();
};
