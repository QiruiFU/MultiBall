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

int64 UMultiBallBlueprintLibrary::GetCurrentRoundScore(const UObject* WorldContextObject)
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

	UScoreSubsystem* ScoreSys = World->GetSubsystem<UScoreSubsystem>();
	if (ScoreSys)
	{
		return ScoreSys->GetRoundScore();
	}
	return 0;
}

FText UMultiBallBlueprintLibrary::GetTotalScoreFormatted(const UObject* WorldContextObject)
{
	int64 Total = GetCurrentRoundScore(WorldContextObject);
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
