// Copyright Autonomix. All Rights Reserved.

#include "BoardCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "BoardActor.h"
#include "Kismet/GameplayStatics.h"

ABoardCameraPawn::ABoardCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	// Spring arm — points along -Y so camera faces the board
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootScene);
	SpringArm->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Look along -Y
	SpringArm->TargetArmLength = 1200.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 8.0f;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Defaults — slightly wider than the board (500 x 800)
	PanSpeed = 600.0f;
	PanBoundsX = 600.0f;
	PanBoundsZ = 900.0f;

	ZoomSpeed = 120.0f;
	MinZoomDistance = 400.0f;
	MaxZoomDistance = 2500.0f;

	bIsRightMouseDown = false;
	LastMousePosition = FVector2D::ZeroVector;
	KeyboardPanInput = FVector2D::ZeroVector;
}

void ABoardCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Auto-find the board and position camera at its center
	ABoardActor* Board = Cast<ABoardActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABoardActor::StaticClass()));
	if (Board)
	{
		BoardCenter = Board->GetActorLocation();
		// Position camera at board center, offset along Y
		FVector StartPos = BoardCenter;
		StartPos.Y += SpringArm->TargetArmLength; // Camera sits in front
		SetActorLocation(FVector(BoardCenter.X, 0.0f, BoardCenter.Z));

		UE_LOG(LogTemp, Log, TEXT("BoardCameraPawn: Found board at (%.0f, %.0f, %.0f). Camera centered."),
		       BoardCenter.X, BoardCenter.Y, BoardCenter.Z);
	}
	else
	{
		BoardCenter = GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("BoardCameraPawn: No BoardActor found. Using spawn location as center."));
	}
}

void ABoardCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// WASD keyboard panning — use direct key bindings
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &ABoardCameraPawn::OnScrollUp).bConsumeInput = false;
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &ABoardCameraPawn::OnScrollDown).bConsumeInput = false;

	// Use lambdas via key held state (we poll keys in Tick instead)
	// No axis needed — Tick checks IsInputKeyDown

	// Scroll wheel zoom
	PlayerInputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &ABoardCameraPawn::OnScrollUp);
	PlayerInputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &ABoardCameraPawn::OnScrollDown);

	// Right mouse button for drag-pan
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ABoardCameraPawn::OnRightMousePressed);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ABoardCameraPawn::OnRightMouseReleased);
}

void ABoardCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	FVector Location = GetActorLocation();

	// --- Keyboard panning (poll held keys) ---
	float PanX = 0.0f;
	float PanZ = 0.0f;

	if (PC->IsInputKeyDown(EKeys::D))
	{
		PanX += 1.0f;
	}
	if (PC->IsInputKeyDown(EKeys::A))
	{
		PanX -= 1.0f;
	}
	if (PC->IsInputKeyDown(EKeys::W))
	{
		PanZ += 1.0f;
	}
	if (PC->IsInputKeyDown(EKeys::S))
	{
		PanZ -= 1.0f;
	}

	if (!FMath::IsNearlyZero(PanX) || !FMath::IsNearlyZero(PanZ))
	{
		Location.X += PanX * PanSpeed * DeltaTime;
		Location.Z += PanZ * PanSpeed * DeltaTime;
	}

	// --- Right-click drag panning ---
	if (bIsRightMouseDown)
	{
		float MouseX, MouseY;
		PC->GetMousePosition(MouseX, MouseY);
		FVector2D CurrentMouse(MouseX, MouseY);

		if (!LastMousePosition.IsZero())
		{
			FVector2D Delta = CurrentMouse - LastMousePosition;
			// Scale drag with zoom level for consistent feel
			float DragScale = SpringArm->TargetArmLength * 0.0015f;
			Location.X -= Delta.X * DragScale;
			Location.Z += Delta.Y * DragScale;
		}

		LastMousePosition = CurrentMouse;
	}

	// --- Clamp position relative to board center ---
	Location.X = FMath::Clamp(Location.X, BoardCenter.X - PanBoundsX, BoardCenter.X + PanBoundsX);
	Location.Z = FMath::Clamp(Location.Z, BoardCenter.Z - PanBoundsZ, BoardCenter.Z + PanBoundsZ);
	SetActorLocation(Location);

	// Debug: show camera info
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green,
			FString::Printf(TEXT("Camera: (%.0f, %.0f, %.0f)  Board: (%.0f, %.0f, %.0f)"),
				Location.X, Location.Y, Location.Z,
				BoardCenter.X, BoardCenter.Y, BoardCenter.Z));
		GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Yellow,
			FString::Printf(TEXT("Z Clamp: [%.0f, %.0f]  X Clamp: [%.0f, %.0f]"),
				BoardCenter.Z - PanBoundsZ, BoardCenter.Z + PanBoundsZ,
				BoardCenter.X - PanBoundsX, BoardCenter.X + PanBoundsX));
	}
}

void ABoardCameraPawn::OnScrollUp()
{
	float NewLength = SpringArm->TargetArmLength - ZoomSpeed;
	SpringArm->TargetArmLength = FMath::Clamp(NewLength, MinZoomDistance, MaxZoomDistance);
}

void ABoardCameraPawn::OnScrollDown()
{
	float NewLength = SpringArm->TargetArmLength + ZoomSpeed;
	SpringArm->TargetArmLength = FMath::Clamp(NewLength, MinZoomDistance, MaxZoomDistance);
}

void ABoardCameraPawn::OnRightMousePressed()
{
	bIsRightMouseDown = true;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		float MouseX, MouseY;
		PC->GetMousePosition(MouseX, MouseY);
		LastMousePosition = FVector2D(MouseX, MouseY);
	}
}

void ABoardCameraPawn::OnRightMouseReleased()
{
	bIsRightMouseDown = false;
	LastMousePosition = FVector2D::ZeroVector;
}
