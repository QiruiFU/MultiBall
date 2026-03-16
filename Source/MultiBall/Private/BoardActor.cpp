// Copyright Autonomix. All Rights Reserved.

#include "BoardActor.h"
#include "PlaceableActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABoardActor::ABoardActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Board bounds
	BoardBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardBounds"));
	BoardBounds->SetBoxExtent(FVector(500.0f, 50.0f, 800.0f));
	BoardBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoardBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetRootComponent(BoardBounds);

	// Visual mesh — flat board
	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	BoardMesh->SetupAttachment(BoardBounds);
	BoardMesh->SetRelativeScale3D(FVector(10.0f, 1.0f, 16.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		BoardMesh->SetStaticMesh(CubeMesh.Object);
	}

	MinPlacementDistance = 30.0f;
}

void ABoardActor::BeginPlay()
{
	Super::BeginPlay();
}

bool ABoardActor::IsLocationValid(FVector Location, TSubclassOf<APlaceableActor> PlaceableClass) const
{
	// Check if location is within board bounds
	FVector LocalPoint = BoardBounds->GetComponentTransform().InverseTransformPosition(Location);
	FVector Extent = BoardBounds->GetUnscaledBoxExtent();

	if (FMath::Abs(LocalPoint.X) > Extent.X ||
	    FMath::Abs(LocalPoint.Y) > Extent.Y ||
	    FMath::Abs(LocalPoint.Z) > Extent.Z)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Board: Location out of bounds."));
		return false;
	}

	// Check minimum distance from existing placeables
	for (const APlaceableActor* Existing : PlacedActors)
	{
		if (Existing && IsValid(Existing))
		{
			float Distance = FVector::Dist(Location, Existing->GetActorLocation());
			if (Distance < MinPlacementDistance)
			{
				UE_LOG(LogTemp, Verbose, TEXT("Board: Too close to existing placeable (%.1f < %.1f)."),
				       Distance, MinPlacementDistance);
				return false;
			}
		}
	}

	return true;
}

void ABoardActor::RegisterPlaceable(APlaceableActor* Placeable)
{
	if (Placeable)
	{
		PlacedActors.AddUnique(Placeable);
		UE_LOG(LogTemp, Log, TEXT("Board: Registered %s. Total: %d"), *Placeable->GetName(), PlacedActors.Num());
	}
}

void ABoardActor::UnregisterPlaceable(APlaceableActor* Placeable)
{
	PlacedActors.Remove(Placeable);
}

void ABoardActor::ClearBoard()
{
	for (APlaceableActor* Placeable : PlacedActors)
	{
		if (Placeable)
		{
			Placeable->Destroy();
		}
	}
	PlacedActors.Empty();
	UE_LOG(LogTemp, Log, TEXT("Board: Cleared all placeables."));
}
