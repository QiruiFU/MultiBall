// Copyright Autonomix. All Rights Reserved.

#include "BumperActor.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"

ABumperActor::ABumperActor()
{
	FString matName = "/Game/RedMat.RedMat";
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> matAsset(*matName);

	UMaterialInterface* mat = matAsset.Object;
	MeshComponent->SetMaterial(0, mat);
	PlaceableType = EPlaceableType::Bumper;
	Cost = 25;
	ChipValue = 2;
	MultiplierValue = 2.0f;
	MaxDurability = 20; // Bumpers wear out
	BounceForce = 800.0f;

	// Larger collision for bumpers
	if (CollisionComponent)
	{
		CollisionComponent->InitSphereRadius(30.0f);
	}
}

void ABumperActor::OnBallHit(ABallActor* Ball)
{
	// Apply base scoring
	Super::OnBallHit(Ball);
	
	if (bIsBroken || !Ball)
	{
		return;
	}

	// Apply bounce impulse away from the bumper center
	FVector Direction = Ball->GetActorLocation() - GetActorLocation();
	Direction.Normalize();

	UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(Ball->GetRootComponent());
	if (BallRoot && BallRoot->IsSimulatingPhysics())
	{
		BallRoot->AddImpulse(Direction * BounceForce);
		UE_LOG(LogTemp, Log, TEXT("Bumper %s bounced ball with force %.0f"),
		       *GetName(), BounceForce);
	}
}
