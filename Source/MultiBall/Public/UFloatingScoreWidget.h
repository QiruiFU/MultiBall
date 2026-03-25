// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UFloatingScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIBALL_API UUFloatingScoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnScoreChanged(int32 NewScore, bool chip);
};
