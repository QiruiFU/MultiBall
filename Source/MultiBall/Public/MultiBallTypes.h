// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MultiBallTypes.generated.h"

class APlaceableActor;

/** Phases of the core game loop. */
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Shop		UMETA(DisplayName = "Shop"),
	Build		UMETA(DisplayName = "Build"),
	Drop		UMETA(DisplayName = "Drop"),
	Rewards		UMETA(DisplayName = "Rewards"),
	GameOver	UMETA(DisplayName = "Game Over")
};

/** Types of placeable components. */
UENUM(BlueprintType)
enum class EPlaceableType : uint8
{
	Peg			UMETA(DisplayName = "Peg"),
	Bumper		UMETA(DisplayName = "Bumper"),
	Rail		UMETA(DisplayName = "Rail")
};

/** Accumulated scoring data (Chips x Multiplier). */
USTRUCT(BlueprintType)
struct FScoreData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Chips = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	float Multiplier = 1.0f;

	/** Computed total: Chips * Multiplier, truncated to int64. */
	int64 GetTotalScore() const { return static_cast<int64>(Chips * Multiplier); }

	void Reset()
	{
		Chips = 0;
		Multiplier = 1.0f;
	}
};

/** An item available in the shop. */
USTRUCT(BlueprintType)
struct FShopItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<APlaceableActor> PlaceableClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 Cost = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	FText Description;
};

/** Data describing a preset opponent. */
USTRUCT(BlueprintType)
struct FOpponentData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Opponent")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Opponent")
	int64 TargetScore = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Opponent")
	int32 Difficulty = 1;
};
