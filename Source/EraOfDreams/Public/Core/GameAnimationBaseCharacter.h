// Copyright (c) 2024 Marvin Bernd Watanabe

#pragma once

#include "GameAnimationBaseCoreTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"
#include "Traversal/TraversableObstacleComponent.h"
#include "GameAnimationBaseCharacter.generated.h"

UCLASS(Abstract)
class ERAOFDREAMS_API AGameAnimationBaseCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class AGameAnimationBasePlayerController;

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TObjectPtr<class USpringArmComponent> SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TObjectPtr<class UMotionWarpingComponent> MotionWarping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	bool bPreviousWantsFirstPerson = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	bool bWantsFirstPerson = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	bool bWantsToSprint = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	bool bWantsToWalk = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	bool bWantsToStrafe = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	bool bWantsToAim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TSoftObjectPtr<UCurveFloat> StrafeSpeedMapCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EGait Gait = EGait::Run;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bJustLanded = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector LandVelocity = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Traversal")
	TSoftObjectPtr<class UChooserTable> TraversalAnimationsChooserTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FGameplayTag JumpEventAudioGameplayTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FGameplayTag LandEventAudioGameplayTag;
	UPROPERTY()
	FTimerHandle LandTimerHandle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Camera")
		virtual void UpdateCamera(bool bInterpolate);

	// This function is called every tick, and is used to update the Gait value and use it to set the max walk speeds of the character movement component.
	UFUNCTION(BlueprintCallable, Category="Movement")
		virtual void UpdateMovement();
	// This function is called every tick, and is used to  update the character movement component’s rotation mode using the Wants to Strafe input condition. When the character is on the ground, this function sets the Rotation Rate to -1, which causes the character to rotate instantly. This technique allows us to treat the actor as the "target rotation", while we independently control the rotation of the root bone within the Animation Blueprint. This allows us to do things not currently supported in the character movement component, such as stick flicks (completely re-orienting the character when only tapping movement input), and gives us more control over the rotation behavior during actions like turn starts, pivots, and turning in place. This is still an experimental technique, and better rotation control should be coming with the new movement component in future engine releases.
	UFUNCTION(BlueprintCallable, Category="Movement")
		virtual void UpdateRotation();
	// This function determines the Gait of the character based on the current input and the Wants to Walk or Wants to Sprint conditions. The Movement Stick Mode determines whether walking or running can be controlled via stick deflection (currently, this can cause issues with motion matching selection. For the safest implementation, use the Fixed Speed - Single Gait option for now).
	UFUNCTION(BlueprintPure, Category="Movement")
		virtual EGait GetDesiredGait() const;
	// This function is used to set the max speed for the character’s movement. Because the forwards, strafes, and backwards animations move at different speeds, we need to change the max speed of the character based on its movement direction. We use a simple curve to map different speed values to the different directions. 0 = forward, 1 = strafe L or R, 2 = Backwards.
	UFUNCTION(BlueprintPure, Category="Movement")
		virtual float CalculateMaxSpeed() const;
	
	UFUNCTION(BlueprintCallable, Category="Traversal")
		virtual void TryTraversalAction(float TraceForwardDistance, bool& bOutTraversalCheckFailed, bool& bOutMontageSelectionFailed);
	// Gets how fast the character is moving in its forward direction and uses the value to scale the distance of the forward trace.
	UFUNCTION(BlueprintPure, Category="Traversal")
		virtual float GetTraversalForwardTraceDistance() const;
	// In order for the actor to move to the exact points on the obstacle, we use a Motion Warping component which warps the montage’s root motion using notify states on the montage. This function updates the warp targets in the component using the ledge locations.
	UFUNCTION(BlueprintCallable, Category="Traversal")
		virtual void UpdateWarpTargets() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
		void PlayAnimationMontage(const UAnimMontage* Montage, float PlayRate, float StartingPosition);
	UFUNCTION(BlueprintCallable, Category="Traversal")
		virtual void OnAnimationMontageCompletedOrInterrupted();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Audio")
		void PlayAudioEvent(const FGameplayTag& Value, float VolumeMultiplier, float PitchMultiplier);

public:
	AGameAnimationBaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	// Event On Jumped - Play a sound whenever the character jumps. This could be done with audio events on the jump animations, but this way gives us more consistent timing, since Motion Matching can pick different entry frames of the jump animations.
	virtual void OnJumped_Implementation() override;
	// Event On Land - Play a sound whenever the character lands similar to Event On Jumped. This logic also caches the land velocity and enables a Just Landed flag, which is used in a Chooser Table asset to select landing databases. We want this flag to be true for more than one frame, so that if movement conditions change upon landing, such as starting or stopping right after impact, landing databases will still be valid.
	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		float CameraDistanceMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		FCameraParameters CameraStyleFirstPerson;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		FCameraParameters CameraStyleThirdPersonFar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		FCameraParameters CameraStyleThirdPersonClose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		FCameraParameters CameraStyleThirdPersonAim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FVector WalkSpeeds = FVector(200.0f, 175.0f, 150.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FVector RunSpeeds = FVector(500.0f, 350.0f, 300.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FVector SprintSpeeds = FVector(700.0f, 700.0f, 700.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FVector CrouchSpeeds = FVector(200.0f, 175.0f, 150.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FRotator ThirdPersonFallingRotationRate = FRotator(0.0f, 200.0f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FRotator ThirdPersonNotFallingRotationRate = FRotator(0.0f, -1.0f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FRotator FirstPersonFallingRotationRate = FRotator(0.0f, -1.0f, 0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		FRotator FirstPersonNotFallingRotationRate = FRotator(0.0f, -1.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal")
		FTraversalCheckResult TraversalResult = {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Traversal")
		bool bDoingTraversalAction = false;
};

UCLASS(meta = (BlueprintThreadSafe))
class ERAOFDREAMS_API UPoseSearchTrajectoryLibraryExtension : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Experimental: Thread-safe variation of UPoseSearchTrajectoryLibrary::HandleTrajectoryWorldCollisions
	UFUNCTION(BlueprintCallable, Category = "Animation|PoseSearch|Experimental", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore", AdvancedDisplay = "TraceChannel,bTraceComplex,ActorsToIgnore,DrawDebugType,bIgnoreSelf,MaxObstacleHeight,TraceColor,TraceHitColor,DrawTime"))
		static void HandleTrajectoryWorldCollisionsThreadSafe(const UObject* WorldContextObject,
		const UAnimInstance* AnimInstance, const FPoseSearchQueryTrajectory& InTrajectory, bool bApplyGravity,
		float FloorCollisionsOffset, FPoseSearchQueryTrajectory& OutTrajectory,
		FPoseSearchTrajectory_WorldCollisionResults& CollisionResult, ETraceTypeQuery TraceChannel, bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf,
		float MaxObstacleHeight, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
};
