// Copyright Autonomix. All Rights Reserved.

#include "BallActor.h"
#include "PlaceableActor.h"
#include "ScoreSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SpecialSkillSubsystem.h"
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

	// Scoring defaults
	BaseChips = 10;
	BaseMultiplier = 1.0f;
	CollisionCooldown = 0.1f;
	AccumulatedChips = 0;
	AccumulatedMultiplier = 1.0f;

	// Settle detection
	SettleVelocityThreshold = 5.0f;
	SettleTimeRequired = 2.0f;
	SettleTimer = 0.0f;
	bHasSettled = false;
	KillZ = 1.0f;
	MaxLifespan = 15.0f;
}

void ABallActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind physics hit event
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABallActor::OnHit);

	// Start lifespan timer
	GetWorld()->GetTimerManager().SetTimer(LifespanTimerHandle, this, &ABallActor::OnLifespanExpired, MaxLifespan, false);

	// Initialize with base stats
	AccumulatedChips = BaseChips;
	AccumulatedMultiplier = BaseMultiplier;

	// Apply skill effects at spawn
	USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
	if (SkillSys)
	{
		// BiggerBalls: scale up
		float ScaleMult = SkillSys->GetBallScaleMultiplier();
		if (ScaleMult > 1.0f)
		{
			SetActorScale3D(GetActorScale3D() * ScaleMult);
		}

		// BonusMultiplier: add to starting multiplier
		float BonusMult = SkillSys->GetBonusMultiplier();
		if (BonusMult > 0.0f)
		{
			AccumulatedMultiplier += BonusMult;
		}
	}
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
		float CurrentTime = GetWorld()->GetTimeSeconds();
		float* LastHitTimePtr = LastHitTimes.Find(OtherActor);

		// Check cooldown
		if (LastHitTimePtr && (CurrentTime - *LastHitTimePtr) < CollisionCooldown)
		{
			// Too soon to hit the same object again
			UE_LOG(LogTemp, Verbose, TEXT("[Collision] %s hit %s but ignored due to Cooldown (%.2fs elapsed)."),
			       *GetName(), *OtherActor->GetName(), (CurrentTime - *LastHitTimePtr));
			return;
		}

		// Update last hit time
		LastHitTimes.Add(OtherActor, CurrentTime);

		Placeable->OnBallHit(this);

		// SplitChance: chance to spawn an extra ball on peg hit
		USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
		if (SkillSys)
		{
			float SplitChance = SkillSys->GetSplitChanceBonus();
			if (SplitChance > 0.0f && FMath::FRand() < SplitChance)
			{
				// Spawn a split ball at current location
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				ABallActor* SplitBall = GetWorld()->SpawnActor<ABallActor>(GetClass(), GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
				if (SplitBall)
				{
					// Give the split ball a random sideways impulse
					FVector Impulse = FVector(FMath::FRandRange(-200.0f, 200.0f), FMath::FRandRange(-200.0f, 200.0f), 100.0f);
					SplitBall->CollisionComponent->AddImpulse(Impulse);
					UE_LOG(LogTemp, Log, TEXT("Ball %s split! New ball spawned."), *GetName());
				}
			}
		}
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

void ABallActor::MultiplyMultiplier(float Amount)
{
	AccumulatedMultiplier *= Amount;
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

	GetWorld()->GetTimerManager().ClearTimer(LifespanTimerHandle);

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

void ABallActor::OnLifespanExpired()
{
	if (!bHasSettled)
	{
		UE_LOG(LogTemp, Log, TEXT("Ball %s expired due to max lifespan (%.1fs). Forcing settle."), *GetName(), MaxLifespan);
		SettleBall();
	}
}
