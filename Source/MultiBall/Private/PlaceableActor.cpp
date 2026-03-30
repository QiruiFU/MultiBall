// Copyright Autonomix. All Rights Reserved.

#include "PlaceableActor.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InteractionRuleComponent.h"

#include "UObject/ConstructorHelpers.h"

APlaceableActor::APlaceableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision primitive (root)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(20.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SetRootComponent(CollisionComponent);

	PlacementBlockingRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PlacementBlockingRadius"));
	PlacementBlockingRadius->InitSphereRadius(60.0f);
	PlacementBlockingRadius->SetCollisionProfileName(TEXT("QueryOnly"));
	PlacementBlockingRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	PlacementBlockingRadius->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	PlacementBlockingRadius->SetGenerateOverlapEvents(true);
	PlacementBlockingRadius->SetupAttachment(CollisionComponent);

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
	MaxDurability = 0; // 0 = indestructible
	CurrentDurability = 0;
	bIsBroken = false;
	bIsFixed = false;
	FixedForRound = 0;

	
	TSubclassOf<UUFloatingScoreWidget> WidgetClass = StaticLoadClass(UUFloatingScoreWidget::StaticClass(), nullptr, TEXT("/Game/UI/WBP_FloatingScoreHolder.WBP_FloatingScoreHolder_C"));
	if (WidgetClass != nullptr)
	{

		WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
		if (WidgetComponent != nullptr)
		{
			WidgetComponent->SetWidgetClass(WidgetClass);
			WidgetComponent->SetupAttachment(RootComponent);
			WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
			
			
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("WidgetCompnentNotCreated"));
		}

	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("WidgetBlueprintNotFound"));
	}
	
}

void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();
	WidgetInstance = Cast<UUFloatingScoreWidget>(WidgetComponent->GetUserWidgetObject());
	
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

	// Execute all attached Interaction Rules
	TArray<UInteractionRuleComponent*> Rules;
	GetComponents<UInteractionRuleComponent>(Rules);
	
	for (UInteractionRuleComponent* Rule : Rules)
	{
		if (Rule)
		{
			Rule->ApplyRule(Ball, this);
		}
	}

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

	FScoreData BallScore = Ball->GetScoreData();
	WidgetInstance->OnScoreChanged(BallScore.GetTotalScore(), false);
	UE_LOG(LogTemp, Log, TEXT("Ball hit %s. Ball total: %d chips x%.2f mult = %lld"),
	       *GetName(), BallScore.Chips, BallScore.Multiplier, BallScore.GetTotalScore());
}
