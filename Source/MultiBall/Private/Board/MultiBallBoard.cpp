// Copyright Autonomix. All Rights Reserved.

#include "Board/MultiBallBoard.h"

// Sets default values
AMultiBallBoard::AMultiBallBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	RootComponent = BoardMesh;
}

// Called when the game starts or when spawned
void AMultiBallBoard::BeginPlay()
{
	Super::BeginPlay();
	
}
