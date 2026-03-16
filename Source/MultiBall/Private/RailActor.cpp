// Copyright Autonomix. All Rights Reserved.

#include "RailActor.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"

ARailActor::ARailActor()
{
	PlaceableType = EPlaceableType::Rail;
	Cost = 30;
	ChipValue = 1;
	MultiplierValue = 1.5f;
	MaxDurability = 15;
	GuideForce = 400.0f;

	// Create spline component
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(GetRootComponent());

	// Use a box-ish collision for rails
	if (CollisionComponent)
	{
		CollisionComponent->InitSphereRadius(25.0f);
	}
}

void ARailActor::OnBallHit(ABallActor* Ball)
{
	// Apply base scoring
	Super::OnBallHit(Ball);

	if (bIsBroken || !Ball || !SplineComponent)
	{
		return;
	}

	// Find the nearest point on the spline and push the ball along the spline tangent
	FVector BallLocation = Ball->GetActorLocation();
	float InputKey = SplineComponent->FindInputKeyClosestToWorldLocation(BallLocation);
	FVector SplineTangent = SplineComponent->GetTangentAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
	SplineTangent.Normalize();

	UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(Ball->GetRootComponent());
	if (BallRoot && BallRoot->IsSimulatingPhysics())
	{
		BallRoot->AddForce(SplineTangent * GuideForce);
		UE_LOG(LogTemp, Verbose, TEXT("Rail %s guiding ball along spline."), *GetName());
	}
}
