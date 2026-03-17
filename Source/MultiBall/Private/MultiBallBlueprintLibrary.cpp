// Copyright Autonomix. All Rights Reserved.

#include "MultiBallBlueprintLibrary.h"
#include "ScoreSubsystem.h"
#include "MultiBallGameMode.h"
#include "MultiBallPlayerState.h"
#include "GameFramework/PlayerController.h"

FVector UMultiBallBlueprintLibrary::GetLocationFromHitResult(const FHitResult& HitResult)
{
	return HitResult.Location;
}

FScoreData UMultiBallBlueprintLibrary::GetCurrentRoundScore(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return FScoreData();
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return FScoreData();
	}

	UScoreSubsystem* ScoreSys = World->GetSubsystem<UScoreSubsystem>();
	if (ScoreSys)
	{
		return ScoreSys->GetRoundScore();
	}
	return FScoreData();
}

FText UMultiBallBlueprintLibrary::GetTotalScoreFormatted(const UObject* WorldContextObject)
{
	FScoreData Score = GetCurrentRoundScore(WorldContextObject);
	int64 Total = Score.GetTotalScore();
	return FText::AsNumber(Total);
}

int32 UMultiBallBlueprintLibrary::GetCurrentRoundNumber(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return 0;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return 0;
	}

	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(World->GetAuthGameMode());
	if (GM)
	{
		return GM->GetCurrentRound();
	}
	return 0;
}

int32 UMultiBallBlueprintLibrary::GetPlayerCoins(APlayerController* PC)
{
	if (!PC)
	{
		return 0;
	}

	AMultiBallPlayerState* PS = PC->GetPlayerState<AMultiBallPlayerState>();
	if (PS)
	{
		return PS->PlayerCoins;
	}
	return 0;
}

int64 UMultiBallBlueprintLibrary::GetOpponentTargetScore(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return 0;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return 0;
	}

	AMultiBallGameMode* GM = Cast<AMultiBallGameMode>(World->GetAuthGameMode());
	if (GM)
	{
		return GM->GetCurrentOpponentTargetScore();
	}
	return 0;
}
