// Copyright Autonomix. All Rights Reserved.

#include "BallActor.h"
#include "PlaceableActor.h"
#include "ScoreSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABallActor::ABallActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision sphere (root)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	SetRootComponent(CollisionComponent);

	// Visual mesh — default sphere
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRelativeScale3D(FVector(0.2f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
	}

	// Scoring
	AccumulatedChips = 0;
	AccumulatedMultiplier = 1.0f;

	// Settle detection
	SettleVelocityThreshold = 5.0f;
	SettleTimeRequired = 2.0f;
	SettleTimer = 0.0f;
	bHasSettled = false;
	KillZ = 1.0f;
}

void ABallActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind physics hit event
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABallActor::OnHit);
}

void ABallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasSettled)
	{
		return;
	}

	// Kill ball if it falls below the kill height
	if (GetActorLocation().Z < KillZ)
	{
		UE_LOG(LogTemp, Log, TEXT("Ball %s fell below KillZ (%.0f). Destroying."), *GetName(), KillZ);
		SettleBall();
		return;
	}

	// Check if ball velocity is below settle threshold
	FVector Velocity = CollisionComponent->GetPhysicsLinearVelocity();
	float Speed = Velocity.Size();

	if (Speed < SettleVelocityThreshold)
	{
		SettleTimer += DeltaTime;
		if (SettleTimer >= SettleTimeRequired)
		{
			SettleBall();
		}
	}
	else
	{
		SettleTimer = 0.0f;
	}
}

void ABallActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                        const FHitResult& Hit)
{
	if (bHasSettled)
	{
		return;
	}

	// If we hit a placeable, let it apply scoring to us
	APlaceableActor* Placeable = Cast<APlaceableActor>(OtherActor);
	if (Placeable)
	{
		Placeable->OnBallHit(this);
	}
}

void ABallActor::AddChips(int32 Amount)
{
	AccumulatedChips += Amount;
}

void ABallActor::AddMultiplier(float Amount)
{
	AccumulatedMultiplier += Amount;
}

FScoreData ABallActor::GetScoreData() const
{
	FScoreData Data;
	Data.Chips = AccumulatedChips;
	Data.Multiplier = AccumulatedMultiplier;
	return Data;
}

void ABallActor::SettleBall()
{
	bHasSettled = true;

	UE_LOG(LogTemp, Log, TEXT("Ball %s settled: %d chips x %.2f mult = %lld total"),
	       *GetName(), AccumulatedChips, AccumulatedMultiplier,
	       static_cast<long long>(AccumulatedChips * AccumulatedMultiplier));

	// Broadcast score finalized
	OnBallScoreFinalized.Broadcast(AccumulatedChips, AccumulatedMultiplier);

	// Add to round score via subsystem
	UScoreSubsystem* ScoreSys = GetWorld()->GetSubsystem<UScoreSubsystem>();
	if (ScoreSys)
	{
		ScoreSys->AddBallScore(AccumulatedChips, AccumulatedMultiplier);
	}

	// Destroy after a short delay for visual feedback
	SetLifeSpan(0.5f);
}
