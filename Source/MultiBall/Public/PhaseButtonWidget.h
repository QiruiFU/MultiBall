// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiBallTypes.h"
#include "PhaseButtonWidget.generated.h"

class UButton;

/**
 * Widget with phase transition buttons. Visibility is managed per-phase.
 * - StartBuildButton: visible in Shop phase
 * - StartDropButton: visible in Build phase
 * - EnterShopButton: visible in Rewards phase
 */
UCLASS()
class MULTIBALL_API UPhaseButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Update which button is visible based on the current phase. */
	UFUNCTION(BlueprintCallable, Category = "Phase")
	void SetPhase(EGamePhase NewPhase);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Phase")
	UButton* StartBuildButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Phase")
	UButton* StartDropButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Phase")
	UButton* EnterShopButton;

private:
	UFUNCTION()
	void OnStartBuildClicked();

	UFUNCTION()
	void OnStartDropClicked();

	UFUNCTION()
	void OnEnterShopClicked();
};
