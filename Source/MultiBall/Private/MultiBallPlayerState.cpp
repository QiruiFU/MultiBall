// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerState.h"
#include "Net/UnrealNetwork.h"

AMultiBallPlayerState::AMultiBallPlayerState()
{
	PlayerCoins = 0;
	RoundChips = 0;
	RoundMultiplier = 1.0f;
	TotalScore = 0;
	CurrentRound = 1;
}

void AMultiBallPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiBallPlayerState, PlayerCoins);
	DOREPLIFETIME(AMultiBallPlayerState, RoundChips);
	DOREPLIFETIME(AMultiBallPlayerState, RoundMultiplier);
	DOREPLIFETIME(AMultiBallPlayerState, TotalScore);
	DOREPLIFETIME(AMultiBallPlayerState, CurrentRound);
}
