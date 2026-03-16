// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "PegActor.generated.h"

/**
 * 
 */
UCLASS()
class MULTIBALL_API APegActor : public APlaceableActor
{
	GENERATED_BODY()

public:
    APegActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
	
};
