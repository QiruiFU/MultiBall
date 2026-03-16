// Copyright Autonomix. All Rights Reserved.

#include "PegActor.h"

APegActor::APegActor()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void APegActor::BeginPlay()
{
    Super::BeginPlay();
}

void APegActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
