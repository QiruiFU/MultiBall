// Copyright Autonomix. All Rights Reserved.

#include "Ball/MultiBallBall.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMultiBallBall::AMultiBallBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetShouldBounce(true);
}

// Called when the game starts or when spawned
void AMultiBallBall::BeginPlay()
{
	Super::BeginPlay();
	
}
