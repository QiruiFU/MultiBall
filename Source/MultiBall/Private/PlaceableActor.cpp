// Copyright Autonomix. All Rights Reserved.

#include "PlaceableActor.h"
#include "BallActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InteractionRuleComponent.h"
#include "SpecialSkillSubsystem.h"
#include "Net/UnrealNetwork.h"

#include "UObject/ConstructorHelpers.h"

APlaceableActor::APlaceableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Collision primitive (root)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(20.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SetRootComponent(CollisionComponent);

	PlacementBlockingRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PlacementBlockingRadius"));
	PlacementBlockingRadius->InitSphereRadius(40.0f);
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
	DurabilityBarComp = nullptr;
	DurabilityBarInstance = nullptr;

	
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

void APlaceableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlaceableActor, MaxDurability);
	DOREPLIFETIME(APlaceableActor, CurrentDurability);
	DOREPLIFETIME(APlaceableActor, bIsBroken);
}

void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();
	WidgetInstance = Cast<UUFloatingScoreWidget>(WidgetComponent->GetUserWidgetObject());
	
	// Initialise durability and health bar from CDO defaults (if set)
	if (MaxDurability > 0)
	{
		InitDurability(MaxDurability);
	}
}

void APlaceableActor::InitDurability(int32 NewMaxDurability)
{
	if (NewMaxDurability <= 0)
	{
		return;
	}

	MaxDurability = NewMaxDurability;
	CurrentDurability = NewMaxDurability;
	bIsBroken = false;

	// Create durability bar widget component if not already created
	if (!DurabilityBarComp)
	{
		DurabilityBarComp = NewObject<UWidgetComponent>(this, TEXT("DurabilityBarComp"));
		if (DurabilityBarComp)
		{
			DurabilityBarComp->SetupAttachment(RootComponent);
			DurabilityBarComp->RegisterComponent();
			DurabilityBarComp->SetWidgetSpace(EWidgetSpace::Screen);
			DurabilityBarComp->SetWidgetClass(UDurabilityBarWidget::StaticClass());
			DurabilityBarComp->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
			DurabilityBarComp->SetDrawSize(FVector2D(60.0f, 10.0f));
			DurabilityBarComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DurabilityBarInstance = Cast<UDurabilityBarWidget>(DurabilityBarComp->GetUserWidgetObject());
		}
	}

	UpdateDurabilityBar();
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
		UpdateDurabilityBar();

		if (CurrentDurability <= 0)
		{
			// PegRevive: chance to revive instead of breaking
			USpecialSkillSubsystem* SkillSys = GetWorld()->GetSubsystem<USpecialSkillSubsystem>();
			float ReviveChance = SkillSys ? SkillSys->GetPegReviveChance() : 0.0f;
			if (ReviveChance > 0.0f && FMath::FRand() < ReviveChance)
			{
				// Revive! Restore durability
				CurrentDurability = MaxDurability;
				UpdateDurabilityBar();
				UE_LOG(LogTemp, Log, TEXT("Placeable %s REVIVED! (%.0f%% chance)"), *GetName(), ReviveChance * 100.0f);
			}
			else
			{
				bIsBroken = true;
				UE_LOG(LogTemp, Log, TEXT("Placeable %s broke after reaching max durability."), *GetName());

				// Hide and disable collision immediately
				SetActorHiddenInGame(true);
				SetActorEnableCollision(false);

				// Destroy after a brief delay (allows any in-flight scoring to finish)
				SetLifeSpan(0.1f);
			}
		}
	}

	FScoreData BallScore = Ball->GetScoreData();
	WidgetInstance->OnScoreChanged(BallScore.GetTotalScore(), false);
	UE_LOG(LogTemp, Log, TEXT("Ball hit %s. Ball total: %d chips x%.2f mult = %lld"),
	       *GetName(), BallScore.Chips, BallScore.Multiplier, BallScore.GetTotalScore());
}

void APlaceableActor::UpdateDurabilityBar()
{
	if (!DurabilityBarComp || MaxDurability <= 0)
	{
		return;
	}

	bool bFullHealth = (CurrentDurability >= MaxDurability);
	DurabilityBarComp->SetVisibility(!bFullHealth);

	if (DurabilityBarInstance)
	{
		float Percent = static_cast<float>(CurrentDurability) / static_cast<float>(MaxDurability);
		DurabilityBarInstance->SetPercent(Percent);
	}
}
