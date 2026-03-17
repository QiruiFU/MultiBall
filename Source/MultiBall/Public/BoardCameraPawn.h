// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BoardCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * A constrained camera pawn that always faces the board.
 * Supports WASD panning, right-click drag, and scroll-wheel zoom.
 * Rotation is locked so the camera always looks along -Y at the board surface.
 */
UCLASS()
class MULTIBALL_API ABoardCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ABoardCameraPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	protected:
	virtual void BeginPlay() override;

	public:

	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	// --- Pan Settings ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Pan")
	float PanSpeed;

	/** Maximum pan distance on the X axis (board width). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Pan")
	float PanBoundsX;

	/** Maximum pan distance on the Z axis (board height). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Pan")
	float PanBoundsZ;

	// --- Zoom Settings ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float ZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float MinZoomDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float MaxZoomDistance;

private:
	// Input — direct key bindings (works with any input system)
	void OnScrollUp();
	void OnScrollDown();
	void OnRightMousePressed();
	void OnRightMouseReleased();

	// Keyboard pan state (tracked in Tick)
	FVector2D KeyboardPanInput;

	// Mouse drag state
	bool bIsRightMouseDown;
	FVector2D LastMousePosition;

	// Board center for relative clamping
	FVector BoardCenter;
};
