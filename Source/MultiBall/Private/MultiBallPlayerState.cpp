// Copyright Autonomix. All Rights Reserved.

#include "MultiBallPlayerState.h"
#include "Net/UnrealNetwork.h"

AMultiBallPlayerState::AMultiBallPlayerState()
{
    PlayerCoins = 0;
}

void AMultiBallPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMultiBallPlayerState, PlayerCoins);
}
