// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiBallTypes.h"
#include "SpecialSkillWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChosen, ESpecialSkill, ChosenSkill);

/**
 * Pure C++ widget that displays 3 skill choices as buttons.
 * Created programmatically during SkillSelect phase.
 */
UCLASS()
class MULTIBALL_API USpecialSkillWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Populate the widget with 3 skill choices. */
	UFUNCTION(BlueprintCallable, Category = "Skill UI")
	void ShowChoices(const TArray<FSpecialSkillData>& Choices);

	/** Broadcast when the player picks a skill. */
	UPROPERTY(BlueprintAssignable, Category = "Skill UI")
	FOnSkillChosen OnSkillChosen;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	/** Slate text blocks for skill names (set during RebuildWidget). */
	TSharedPtr<STextBlock> TitleText;
	TArray<TSharedPtr<STextBlock>> ButtonTexts;
	TArray<TSharedPtr<STextBlock>> DescTexts;

	/** The 3 skill choices currently displayed. */
	TArray<FSpecialSkillData> CurrentChoices;

	/** Button click handlers. */
	FReply OnSkillButtonClicked(int32 Index);
};
