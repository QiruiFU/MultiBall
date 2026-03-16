// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiBallPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIBALL_API AMultiBallPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    AMultiBallPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State")
    int32 PlayerCoins;
	
};
