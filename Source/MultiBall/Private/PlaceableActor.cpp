// Copyright Autonomix. All Rights Reserved.

#include "PlaceableActor.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APlaceableActor::APlaceableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision primitive (root)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(20.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SetRootComponent(CollisionComponent);

	// Visual mesh — default cylinder
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CylinderMesh.Object);
	}

	// Defaults
	Cost = 10;
	PlaceableType = EPlaceableType::Peg;
	ChipValue = 1;
	MultiplierValue = 1.0f;
	MaxDurability = 0; // 0 = indestructible
	CurrentDurability = 0;
	bIsBroken = false;
}

void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialise current durability from max
	if (MaxDurability > 0)
	{
		CurrentDurability = MaxDurability;
	}
}

void APlaceableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlaceableActor::OnBallHit(ABallActor* Ball)
{
	if (bIsBroken || !Ball)
	{
		return;
	}

	// Apply scoring to the ball
	Ball->AddChips(ChipValue);
	Ball->AddMultiplier(MultiplierValue);

	// Decrement durability
	if (MaxDurability > 0)
	{
		CurrentDurability--;
		if (CurrentDurability <= 0)
		{
			bIsBroken = true;
			UE_LOG(LogTemp, Log, TEXT("Placeable %s broke after reaching max durability."), *GetName());
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT("Ball hit %s: +%d chips, x%.2f mult"), *GetName(), ChipValue, MultiplierValue);
}
