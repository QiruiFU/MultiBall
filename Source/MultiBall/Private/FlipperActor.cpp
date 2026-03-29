// Copyright Autonomix. All Rights Reserved.

#include "FlipperActor.h"
#include "MultiBallPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AFlipperActor::AFlipperActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PlaceableType = EPlaceableType::Flipper;
	Cost = 30; // Flippers are a bit more expensive
	MaxDurability = 0; // Indestructible

	MinAngle = -20.0f;
	MaxAngle = 30.0f;
	FlapSpeed = 1000.0f; // Very fast rotation
	bIsFlipped = false;

	CurrentAngle = MinAngle;
	TargetAngle = MinAngle;

	// Scale the default cylinder visual to look somewhat like a flipper stick
	// We'll scale it to be long and flat
	if (MeshComponent)
	{
		MeshComponent->SetRelativeScale3D(FVector(1.0f, 0.2f, 0.2f));
		// Offset the mesh so it rotates around one end, not the center
		// Z=-20 cancels out the hovering height from the PurchasePlaceable logic
		MeshComponent->SetRelativeLocation(FVector(50.0f, 0.0f, -20.0f));
	}
	
	// Flipper usually doesn't need a huge underlying default sphere, 
	// but the mesh's physical collision blocking will do the actual sweeping.
}

void AFlipperActor::BeginPlay()
{
	Super::BeginPlay();

	// Apply the flip state (may have been set before spawn)
	ApplyFlipVisual();

	BaseSpawnRotation = GetActorQuat();
	CurrentAngle = bIsFlipped ? -MinAngle : MinAngle;
	TargetAngle = CurrentAngle;

	// Initial rotation applied directly to the root component around the local Z (Yaw) axis
	FQuat InitialRot = BaseSpawnRotation * FQuat(FRotator(0.f, CurrentAngle, 0.f));
	if (CollisionComponent)
	{
		CollisionComponent->SetWorldRotation(InitialRot);
	}

	// Subscribe to Player Controller events
	AMultiBallPlayerController* PC = Cast<AMultiBallPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->OnSpacebarAction.AddDynamic(this, &AFlipperActor::OnSpacebarChanged);
	}
}

void AFlipperActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperActor, bIsFlipped);
}

void AFlipperActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 0.1f))
	{
		CurrentAngle = FMath::FInterpConstantTo(CurrentAngle, TargetAngle, DeltaTime, FlapSpeed);

		// Apply exact rotation relative to our spawn tilt around Z-axis (Yaw), and physically sweep!
		FQuat NewRot = BaseSpawnRotation * FQuat(FRotator(0.f, CurrentAngle, 0.f));
		if (CollisionComponent)
		{
			CollisionComponent->SetWorldRotation(NewRot, true);
		}
	}
}

void AFlipperActor::OnSpacebarChanged(bool bIsPressed)
{
	float EffectiveMin = bIsFlipped ? -MinAngle : MinAngle;
	float EffectiveMax = bIsFlipped ? -MaxAngle : MaxAngle;

	if (bIsPressed)
	{
		TargetAngle = EffectiveMax;
	}
	else
	{
		TargetAngle = EffectiveMin;
	}
}

void AFlipperActor::SetFlipped(bool bFlipped)
{
	bIsFlipped = bFlipped;
	ApplyFlipVisual();

	// Re-apply rotation if we've already started (BaseSpawnRotation is set in BeginPlay)
	if (HasActorBegunPlay())
	{
		float EffectiveMin = bIsFlipped ? -MinAngle : MinAngle;
		CurrentAngle = EffectiveMin;
		TargetAngle = EffectiveMin;

		FQuat NewRot = BaseSpawnRotation * FQuat(FRotator(0.f, CurrentAngle, 0.f));
		if (CollisionComponent)
		{
			CollisionComponent->SetWorldRotation(NewRot);
		}
	}
}

void AFlipperActor::ApplyFlipVisual()
{
	if (MeshComponent)
	{
		FVector CurrentLoc = MeshComponent->GetRelativeLocation();

		// Mirror the X offset so the arm extends the other direction
		float AbsX = FMath::Abs(CurrentLoc.X);
		CurrentLoc.X = bIsFlipped ? -AbsX : AbsX;
		MeshComponent->SetRelativeLocation(CurrentLoc);
	}
}

