// Copyright Autonomix. All Rights Reserved.

#include "BallEmitterActor.h"
#include "BallActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ABallEmitterActor::ABallEmitterActor()
{
	PrimaryActorTick.bCanEverTick = true;

	EmitterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EmitterMesh"));
	EmitterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EmitterMesh->SetRelativeScale3D(FVector(0.5f, 2.0f, 0.3f));
	SetRootComponent(EmitterMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		EmitterMesh->SetStaticMesh(CubeMesh.Object);
	}

	// Default ball class — works out of the box
	BallClass = ABallActor::StaticClass();

	// Defaults
	DropInterval = 1.5f;
	BallsPerRound = 5;
	OscillationSpeed = 2.0f;
	OscillationRange = 200.0f;
	bManualDrop = true;

	// State
	bIsDropping = false;
	BallsRemaining = 0;
	DropTimer = 0.0f;
	ActiveBallCount = 0;
	OscillationTime = 0.0f;
}

void ABallEmitterActor::BeginPlay()
{
	Super::BeginPlay();
	OriginLocation = GetActorLocation();
}

void ABallEmitterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Oscillate horizontally
	OscillationTime += DeltaTime;
	FVector NewLocation = OriginLocation;
	NewLocation.X += FMath::Sin(OscillationTime * OscillationSpeed) * OscillationRange;
	SetActorLocation(NewLocation);

	// Handle automatic drop sequence
	if (bIsDropping && BallsRemaining > 0 && !bManualDrop)
	{
		DropTimer += DeltaTime;
		if (DropTimer >= DropInterval)
		{
			DropTimer = 0.0f;
			DropBall();
			BallsRemaining--;
			OnBallsRemainingChanged.Broadcast(BallsRemaining);

			if (BallsRemaining <= 0)
			{
				bIsDropping = false;
				UE_LOG(LogTemp, Log, TEXT("Emitter: All balls dropped. Waiting for them to settle..."));
			}
		}
	}
}

void ABallEmitterActor::StartDropSequence(int32 NumBalls)
{
	BallsRemaining = NumBalls > 0 ? NumBalls : BallsPerRound;
	bIsDropping = true;
	ActiveBallCount = 0;

	// Broadcast initial count
	OnBallsRemainingChanged.Broadcast(BallsRemaining);

	if (!bManualDrop)
	{
		DropTimer = DropInterval; // Drop the first ball immediately
	}

	UE_LOG(LogTemp, Log, TEXT("Emitter: Starting drop sequence with %d balls."), BallsRemaining);
}

void ABallEmitterActor::StopDropSequence()
{
	bIsDropping = false;
	BallsRemaining = 0;
	OnBallsRemainingChanged.Broadcast(BallsRemaining);
	UE_LOG(LogTemp, Log, TEXT("Emitter: Drop sequence stopped."));
}

void ABallEmitterActor::ManualDropBall()
{
	if (bIsDropping && BallsRemaining > 0)
	{
		DropBall();
		BallsRemaining--;
		OnBallsRemainingChanged.Broadcast(BallsRemaining);

		if (BallsRemaining <= 0)
		{
			bIsDropping = false;
			UE_LOG(LogTemp, Log, TEXT("Emitter: All balls manually dropped. Waiting for them to settle..."));
		}
	}
}

void ABallEmitterActor::DropBall()
{
	if (!BallClass || !BallClass->IsChildOf(AActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Emitter: No valid BallClass set! Prevented crash."));
		return;
	}

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	ABallActor* NewBall = GetWorld()->SpawnActor<ABallActor>(BallClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (NewBall)
	{
		ActiveBallCount++;
		NewBall->OnDestroyed.AddDynamic(this, &ABallEmitterActor::OnBallDestroyed);

		UE_LOG(LogTemp, Log, TEXT("Emitter: Dropped ball. Active: %d"), ActiveBallCount);
	}
}

void ABallEmitterActor::OnBallDestroyed(AActor* DestroyedActor)
{
	ActiveBallCount--;
	UE_LOG(LogTemp, Log, TEXT("Emitter: Ball destroyed. Active: %d, Remaining: %d"), ActiveBallCount, BallsRemaining);

	// If no more balls are active and none are remaining, signal completion
	if (ActiveBallCount <= 0 && BallsRemaining <= 0 && !bIsDropping)
	{
		UE_LOG(LogTemp, Log, TEXT("Emitter: All balls finished!"));
		OnAllBallsFinished.Broadcast();
	}
}
