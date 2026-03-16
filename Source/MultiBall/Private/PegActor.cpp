// Copyright Autonomix. All Rights Reserved.

#include "PegActor.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"

APegActor::APegActor()
{
	PlaceableType = EPlaceableType::Peg;
	Cost = 10;
	ChipValue = 5;
	MultiplierValue = 1.0f;
	MaxDurability = 0; // Pegs are indestructible by default

	// Smaller collision for pegs
	if (CollisionComponent)
	{
		CollisionComponent->InitSphereRadius(15.0f);
	}
}

void APegActor::BeginPlay()
{
	Super::BeginPlay();
}

void APegActor::OnBallHit(ABallActor* Ball)
{
	// Call base scoring logic
	Super::OnBallHit(Ball);

	// Peg-specific visual feedback can be added here
	UE_LOG(LogTemp, Log, TEXT("Peg %s was hit!"), *GetName());
}
