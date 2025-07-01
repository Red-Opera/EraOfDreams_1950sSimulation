// Copyright (c) 2024 Marvin Bernd Watanabe

#include "Core/GameAnimationBaseCharacter.h"

#include "AnimationWarpingLibrary.h"
#include "ChooserFunctionLibrary.h"
#include "GameAnimationBasePlayerControllerInterface.h"
#include "KismetAnimationLibrary.h"
#include "MotionWarpingComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "Traversal/InteractionTransformInterface.h"

AGameAnimationBaseCharacter::AGameAnimationBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	USkeletalMeshComponent* CharacterMeshComponent = GetMesh();
	CharacterMeshComponent->SetRelativeTransform(FTransform(
		FRotator(0.0f, -90.0f, 0.0f).Quaternion(),
		FVector(0.0f, 0.0f, -88.0f),
		FVector::OneVector));
	CharacterMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->MaxAcceleration = 800.0f;
	CharacterMovementComponent->BrakingFrictionFactor = 1.0f;
	CharacterMovementComponent->GroundFriction = 5.0f;
	CharacterMovementComponent->MaxWalkSpeed = 500.0f;
	CharacterMovementComponent->MinAnalogWalkSpeed = 150.0f;
	CharacterMovementComponent->BrakingDecelerationWalking = 750.0f;
	CharacterMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
	CharacterMovementComponent->PerchRadiusThreshold = 20.0f;
	CharacterMovementComponent->bUseFlatBaseForFloorChecks = true;
	CharacterMovementComponent->JumpZVelocity = 500.0f;
	CharacterMovementComponent->AirControl = 0.25f;
	CharacterMovementComponent->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
	CharacterMovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;

	UCapsuleComponent* CharacterCapsuleComponent = GetCapsuleComponent();
	CharacterCapsuleComponent->SetCapsuleHalfHeight(86.0f);
	CharacterCapsuleComponent->SetCapsuleRadius(30.0f);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetRelativeTransform(FTransform(
		FRotator(0.0f, 0.0f, 0.0f).Quaternion(),
		FVector(0.0f, 0.0f, 20.0f),
		FVector::OneVector));
	SpringArm->ProbeSize = 0.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagMaxDistance = 200.0f;
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));

	CameraStyleFirstPerson.SpringArmLength = 0.0f;
	CameraStyleFirstPerson.SocketOffset = FVector(11.0f, 0.0f, 9.0f);
	CameraStyleFirstPerson.TranslationLagSpeed = 50.0f;
	CameraStyleFirstPerson.FieldOfView = 90.0f;
	CameraStyleFirstPerson.TransitionSpeed = 5.0f;

	CameraStyleThirdPersonFar.SpringArmLength = 300.0f;
	CameraStyleThirdPersonFar.SocketOffset = FVector(0.0f, 0.0f, 20.0f);
	CameraStyleThirdPersonFar.TranslationLagSpeed = 6.0f;
	CameraStyleThirdPersonFar.FieldOfView = 90.0f;
	CameraStyleThirdPersonFar.TransitionSpeed = 2.0f;

	CameraStyleThirdPersonClose.SpringArmLength = 225.0f;
	CameraStyleThirdPersonClose.SocketOffset = FVector(0.0f, 45.0f, 20.0f);
	CameraStyleThirdPersonClose.TranslationLagSpeed = 10.0f;
	CameraStyleThirdPersonClose.FieldOfView = 90.0f;
	CameraStyleThirdPersonClose.TransitionSpeed = 5.0f;

	CameraStyleThirdPersonAim.SpringArmLength = 200.0f;
	CameraStyleThirdPersonAim.SocketOffset = FVector(0.0f, 50.0f, 30.0f);
	CameraStyleThirdPersonAim.TranslationLagSpeed = 15.0f;
	CameraStyleThirdPersonAim.FieldOfView = 80.0f;
	CameraStyleThirdPersonAim.TransitionSpeed = 5.0f;
}

void AGameAnimationBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGameAnimationBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement();
	UpdateRotation();
	UpdateCamera(true);
}

void AGameAnimationBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGameAnimationBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UpdateCamera(false);
}

void AGameAnimationBaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	static const FVector2D RangeA = FVector2D(0.0f, 500.0f);
	static const FVector2D RangeB = FVector2D(0.5f, 1.0f);
	PlayAudioEvent(JumpEventAudioGameplayTag,
		FMath::GetMappedRangeValueClamped(RangeA, RangeB,
			GetCharacterMovement()->Velocity.Size2D()), 1.0f);
}

void AGameAnimationBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	LandVelocity = GetCharacterMovement()->Velocity;
	static const FVector2D RangeA = FVector2D(-500.0f, -900.0f);
	static const FVector2D RangeB = FVector2D(0.5f, 1.5f);
	PlayAudioEvent(LandEventAudioGameplayTag,
	FMath::GetMappedRangeValueClamped(RangeA, RangeB,
		LandVelocity.Z), 1.0f);

	bJustLanded = true;
	GetWorldTimerManager().ClearTimer(LandTimerHandle);
	GetWorldTimerManager().SetTimer(LandTimerHandle, [&](){ bJustLanded = false; }, 0.3f, false);
}

void AGameAnimationBaseCharacter::UpdateCamera(bool bInterpolate)
{
	if (bPreviousWantsFirstPerson != bWantsFirstPerson)
	{
		static const FName HeadSocketName = FName(TEXT("head"));
		SpringArm->AttachToComponent(
			bWantsFirstPerson ? Cast<UPrimitiveComponent>(GetMesh()) : Cast<UPrimitiveComponent>(GetCapsuleComponent()),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			bWantsFirstPerson ? HeadSocketName : NAME_None);
		GetCapsuleComponent()->SetCapsuleSize(bWantsFirstPerson ? 50.0f : 30.0f, 86.0f, true);
		if (!bWantsFirstPerson)
		{
			SpringArm->SetRelativeTransform(FTransform(
				FRotator(0.0f, 0.0f, 0.0f).Quaternion(),
				FVector(0.0f, 0.0f, 20.0f),
				FVector::OneVector));
		}
		bPreviousWantsFirstPerson = bWantsFirstPerson;
	}

	const FCameraParameters& TargetCameraParameters = bWantsFirstPerson ? CameraStyleFirstPerson :
		bWantsToStrafe ? (bWantsToAim ? CameraStyleThirdPersonAim : CameraStyleThirdPersonClose) :
			CameraStyleThirdPersonFar;
	const float TranslationLagSpeed = bInterpolate ? TargetCameraParameters.TranslationLagSpeed : -1.0f; 
	const float TransitionSpeed = bInterpolate ? TargetCameraParameters.TransitionSpeed : -1.0f; 

	Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetCameraParameters.FieldOfView,
		GetWorld()->DeltaTimeSeconds, TransitionSpeed));

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength,
		TargetCameraParameters.SpringArmLength, GetWorld()->DeltaTimeSeconds, TransitionSpeed);
	SpringArm->CameraLagSpeed = FMath::FInterpTo(SpringArm->CameraLagSpeed, TranslationLagSpeed,
		GetWorld()->DeltaTimeSeconds, TransitionSpeed);
	SpringArm->SocketOffset = FMath::VInterpTo(SpringArm->SocketOffset, TargetCameraParameters.SocketOffset,
		GetWorld()->DeltaTimeSeconds, TransitionSpeed);
}

void AGameAnimationBaseCharacter::UpdateMovement()
{
	Gait = GetDesiredGait();
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->MaxWalkSpeed = CharacterMovementComponent->MaxWalkSpeedCrouched =
			CalculateMaxSpeed();
	}
}

void AGameAnimationBaseCharacter::UpdateRotation()
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->bUseControllerDesiredRotation = bWantsFirstPerson || bWantsToStrafe;
		CharacterMovementComponent->bOrientRotationToMovement = !bWantsFirstPerson && !bWantsToStrafe;
		CharacterMovementComponent->RotationRate = CharacterMovementComponent->IsFalling() ?
			(bWantsFirstPerson ? FirstPersonFallingRotationRate : ThirdPersonFallingRotationRate) :
			(bWantsFirstPerson ? FirstPersonNotFallingRotationRate : ThirdPersonNotFallingRotationRate);
	}
}

EGait AGameAnimationBaseCharacter::GetDesiredGait() const
{
	bool bFullMovementInput = false;
	if (IsValid(Controller))
	{
		if (const IGameAnimationBasePlayerControllerInterface* ControllerInterface =
			Cast<IGameAnimationBasePlayerControllerInterface>(Controller))
		{
			bFullMovementInput = ControllerInterface->Execute_IsFullMovementInput(Controller);
		}
		else if (Controller->Implements<UGameAnimationBasePlayerControllerInterface>())
		{
			bFullMovementInput = IGameAnimationBasePlayerControllerInterface::Execute_IsFullMovementInput(Controller);
		}
	}
	return (!bWantsToSprint && bWantsToWalk) || (!bWantsToSprint && !bWantsToWalk && !bFullMovementInput) ?
		EGait::Walk : EGait::Run;
}

float AGameAnimationBaseCharacter::CalculateMaxSpeed() const
{
	if (!StrafeSpeedMapCurve.IsNull())
	{
		const float StrafeSpeedMap = StrafeSpeedMapCurve.LoadSynchronous()->GetFloatValue(
			FMath::Abs(UKismetAnimationLibrary::CalculateDirection(GetCharacterMovement()->Velocity, GetActorRotation())));
		const bool bStrafe = StrafeSpeedMap < 1.0f;
		const FVector& Speeds = GetCharacterMovement()->IsCrouching() ? CrouchSpeeds :
			Gait == EGait::Walk ? WalkSpeeds :
			Gait == EGait::Run ? RunSpeeds :
			Gait == EGait::Sprint ? SprintSpeeds :
			FVector::ZeroVector;
		static const FVector2D RangeA1 = FVector2D(0.0f, 1.0f);
		static const FVector2D RangeA2 = FVector2D(1.0f, 2.0f);
		return FMath::GetMappedRangeValueClamped(bStrafe ? RangeA1 : RangeA2,
			FVector2D(bStrafe ? Speeds.X : Speeds.Y, bStrafe ? Speeds.Y : Speeds.Z), StrafeSpeedMap);
	}
	return INDEX_NONE;
}

void AGameAnimationBaseCharacter::TryTraversalAction(float TraceForwardDistance, bool& bOutTraversalCheckFailed, bool& bOutMontageSelectionFailed) 
{
	// Step 1: Cache some important values for use later in the function.
	const double StartTime = FPlatformTime::Seconds();
	const FVector& ActorLocation = GetActorLocation();
	const FVector& ActorForwardVector = GetActorForwardVector();
	const float& CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float& CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	static const FString DrawDebugLevelConsoleVariableName = TEXT("DDCvar.Traversal.DrawDebugLevel");
	const int32& DrawDebugLevel = UKismetSystemLibrary::GetConsoleVariableIntValue(DrawDebugLevelConsoleVariableName);
	static const FString DrawDebugDurationConsoleVariableName = TEXT("DDCvar.Traversal.DrawDebugDuration");
	const float& DrawDebugDuration = UKismetSystemLibrary::GetConsoleVariableFloatValue(DrawDebugDurationConsoleVariableName);
	FTraversalCheckResult TraversalCheckResult;

	// Step 2.1: Perform a trace in the actor's forward direction to find a Traversable Level Block. If found, set the Hit Component, if not, exit the function.
	FHitResult Hit;
	UKismetSystemLibrary::CapsuleTraceSingle(this, ActorLocation,
		ActorLocation + ActorForwardVector * TraceForwardDistance, 30.0f, 60.0f,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {},
		DrawDebugLevel >= 2 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true,
		FLinearColor::Black, FLinearColor::Black, DrawDebugDuration);

	ITraversableObstacleInterface* Obstacle = Cast<ITraversableObstacleInterface>(Hit.GetActor());
	if (!Hit.bBlockingHit || (Obstacle == nullptr && !Hit.GetActor()->Implements<UTraversableObstacleInterface>()))
	{
		bOutTraversalCheckFailed = true;
		bOutMontageSelectionFailed = false;
		return;
	}
	TraversalCheckResult.HitComponent = Hit.GetComponent();

	// Step 2.2: If a traversable level block was found, get the front and back ledge transforms from it (using its own internal function).
	if (Obstacle != nullptr)
	{
		Obstacle->Execute_GetLedgeTransforms(Hit.GetActor(), Hit.ImpactPoint, ActorLocation, TraversalCheckResult);
	}
	else
	{
		ITraversableObstacleInterface::Execute_GetLedgeTransforms(Hit.GetActor(), Hit.ImpactPoint, ActorLocation,
			TraversalCheckResult);
	}

	// DEBUG: Draw Debug shapes at ledge locations.
	if (DrawDebugLevel >= 1)
	{
		if (TraversalCheckResult.bHasFrontLedge)
		{
			DrawDebugSphere(GetWorld(), TraversalCheckResult.FrontLedgeLocation, 10.0f, 12,
				FLinearColor::Green.ToFColor(true), false, DrawDebugDuration,
				SDPG_World, 1.0f);
		}

		if (TraversalCheckResult.bHasBackLedge)
		{
			DrawDebugSphere(GetWorld(), TraversalCheckResult.BackLedgeLocation, 10.0f, 12,
				FLinearColor::Blue.ToFColor(true), false, DrawDebugDuration,
				SDPG_World, 1.0f);
		}
	}

	// Step 3.1 If the traversable level block has a valid front ledge, continue the function. If not, exit early.
	if (!TraversalCheckResult.bHasFrontLedge)
	{
		bOutTraversalCheckFailed = true;
		bOutMontageSelectionFailed = false;
		return;
	}

	// Step 3.2: Perform a trace from the actors location up to the front ledge location to determine if there is room for the actor to move up to it. If so, continue the function. If not, exit early.
	const FVector HasRoomCheckFrontLedgeLocation = TraversalCheckResult.FrontLedgeLocation +
		TraversalCheckResult.FrontLedgeNormal * (CapsuleRadius + 2.0f) +
		FVector::ZAxisVector * (CapsuleHalfHeight + 2.0f);
	UKismetSystemLibrary::CapsuleTraceSingle(this, ActorLocation,
		HasRoomCheckFrontLedgeLocation, CapsuleRadius, CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {},
		DrawDebugLevel >= 3 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true,
		FLinearColor::Red, FLinearColor::Green, DrawDebugDuration);

	if (Hit.bBlockingHit || Hit.bStartPenetrating)
	{
		TraversalCheckResult.bHasFrontLedge = false;
		bOutTraversalCheckFailed = true;
		bOutMontageSelectionFailed = false;
		return;
	}

	// Step 3.3: save the height of the obstacle using the delta between the actor and front ledge transform.
	TraversalCheckResult.ObstacleHeight = FMath::Abs((ActorLocation - FVector::ZAxisVector * CapsuleHalfHeight - TraversalCheckResult.FrontLedgeLocation).Z);

	// Step 3.4: Perform a trace across the top of the obstacle from the front ledge to the back ledge to see if there's room for the actor to move across it.
	const FVector HasRoomCheckBackLedgeLocation = TraversalCheckResult.BackLedgeLocation +
        TraversalCheckResult.BackLedgeNormal * (CapsuleRadius + 2.0f) +
        FVector::ZAxisVector * (CapsuleHalfHeight + 2.0f);
	if (UKismetSystemLibrary::CapsuleTraceSingle(this, HasRoomCheckFrontLedgeLocation,
		HasRoomCheckBackLedgeLocation, CapsuleRadius, CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {},
		DrawDebugLevel >= 3 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true,
		FLinearColor::Red, FLinearColor::Green, DrawDebugDuration))
	{
		// Step 3.5: If there is no room, save the obstacle depth using the difference between the front ledge and the trace impact point, and invalidate the back ledge.
		TraversalCheckResult.ObstacleDepth = (Hit.ImpactPoint - TraversalCheckResult.FrontLedgeLocation).Size2D();
		TraversalCheckResult.bHasBackLedge = false;
	}
	else
	{
		// Step 3.5: If there is room, save the obstacle depth using the difference between the front and back ledge locations.
		TraversalCheckResult.ObstacleDepth =
			(TraversalCheckResult.FrontLedgeLocation - TraversalCheckResult.BackLedgeLocation).Size2D();

		// Step 3.6: Trace downward from the back ledge location (using the height of the obstacle for the distance) to find the floor. If there is a floor, save its location and the back ledges height (using the distance between the back ledge and the floor). If no floor was found, invalidate the back floor.
		const FVector EndTraceLocation = TraversalCheckResult.BackLedgeLocation +
			TraversalCheckResult.BackLedgeNormal * (CapsuleRadius + 2.0f) -
			FVector::ZAxisVector * (TraversalCheckResult.ObstacleHeight - CapsuleHalfHeight + 50.0f);
		UKismetSystemLibrary::CapsuleTraceSingle(this, HasRoomCheckBackLedgeLocation,
			EndTraceLocation, CapsuleRadius, CapsuleHalfHeight,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {},
			DrawDebugLevel >= 3 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true,
			FLinearColor::Red, FLinearColor::Green, DrawDebugDuration);
		if (Hit.bBlockingHit)
		{
			TraversalCheckResult.bHasBackFloor = true;
			TraversalCheckResult.BackFloorLocation = Hit.ImpactPoint;
			TraversalCheckResult.BackLedgeHeight = FMath::Abs((Hit.ImpactPoint - TraversalCheckResult.BackLedgeLocation).Z);
		}
		else
		{
			TraversalCheckResult.bHasBackFloor = false;
		}
	}

	// Step 4.1: Based on the resulting conditions and parameters of the check, determine what type of traversal action to do.
	if (TraversalCheckResult.bHasFrontLedge && TraversalCheckResult.bHasBackLedge && !TraversalCheckResult.bHasBackFloor &&
		UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 125.0f) &&
		TraversalCheckResult.ObstacleDepth < 59.0f)
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Vault;
	}
	else if (TraversalCheckResult.bHasFrontLedge && TraversalCheckResult.bHasBackLedge && TraversalCheckResult.bHasBackFloor &&
		UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 125.0f) &&
		TraversalCheckResult.ObstacleDepth < 59.0f && TraversalCheckResult.BackLedgeHeight > 50.0f)
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Hurdle;
	}
	else if (TraversalCheckResult.bHasFrontLedge &&
		UKismetMathLibrary::InRange_FloatFloat(TraversalCheckResult.ObstacleHeight, 50.0f, 275.0f) &&
		TraversalCheckResult.ObstacleDepth >= 59.0f)
	{
		TraversalCheckResult.ActionType = ETraversalActionType::Mantle;
	}
	else
	{
		TraversalCheckResult.ActionType = ETraversalActionType::None;
	}

	// Step 5.1: Continue if there is a valid action type. If none of the conditions were met, no action can be performed, therefore exit the function.
	if (TraversalCheckResult.ActionType == ETraversalActionType::None)
	{
		bOutTraversalCheckFailed = true;
		bOutMontageSelectionFailed = false;
		return;
	}
	
	// Step 5.2: Send the front ledge location to the Anim BP using an interface. This transform will be used for a custom channel within the following Motion Matching search.
	IInteractionTransformInterface* InteractableObject =
		Cast<IInteractionTransformInterface>(GetMesh()->GetAnimInstance());

	const FTransform InteractionTransform =
		FTransform(FRotationMatrix::MakeFromZ(TraversalCheckResult.FrontLedgeNormal).ToQuat(),
			TraversalCheckResult.FrontLedgeLocation, FVector::OneVector);
	if (InteractableObject != nullptr)
	{
		InteractableObject->Execute_SetInteractionTransform(GetMesh()->GetAnimInstance(), InteractionTransform);
	}
	else
	{
		//IInteractionTransformInterface::Execute_SetInteractionTransform(GetMesh()->GetAnimInstance(), InteractionTransform);
	}

	// Step 5.3: Evaluate a chooser to select all montages that match the conditions of the traversal check.
	FTraversalChooserParameters ChooserParameters;
	ChooserParameters.ActionType = TraversalCheckResult.ActionType;
	ChooserParameters.Gait = Gait;
	ChooserParameters.Speed = GetCharacterMovement()->Velocity.Size2D();
	ChooserParameters.ObstacleHeight = TraversalCheckResult.ObstacleHeight;
	ChooserParameters.ObstacleDepth = TraversalCheckResult.ObstacleDepth;
	FChooserEvaluationContext Context = UChooserFunctionLibrary::MakeChooserEvaluationContext();
	Context.AddStructParam(ChooserParameters);
	TArray<UObject*> AnimationAssets = UChooserFunctionLibrary::EvaluateObjectChooserBaseMulti(Context,
		UChooserFunctionLibrary::MakeEvaluateChooser(TraversalAnimationsChooserTable.LoadSynchronous()),
		UAnimMontage::StaticClass());

	// Step 5.4: Perform a Motion Match on all the montages that were chosen by the chooser to find the best result. This match will elect the best montage AND the best entry frame (start time) based on the distance to the ledge, and the current characters pose. If for some reason no montage was found (motion matching failed, perhaps due to an invalid database or issue with the schema), print a warning and exit the function.
	static const FName PoseHistoryName = TEXT("PoseHistory");
	FPoseSearchBlueprintResult Result;

	FPoseSearchContinuingProperties ContinuingProps;  // 기본값이면 그대로 비워둡니다.

	// ─────────── MotionMatch 호출 ───────────
	UPoseSearchLibrary::MotionMatch(
		GetMesh()->GetAnimInstance(),   // AnimInstance
		AnimationAssets,                // AssetsToSearch
		PoseHistoryName,                // PoseHistoryName
		ContinuingProps,                // ★ 새로 추가된 인수
		FPoseSearchFutureProperties(),	// Future
		Result                          // Out-param
	);

	TObjectPtr<const UAnimMontage> AnimationMontage = Cast<UAnimMontage>(Result.SelectedAnimation);
	if (!IsValid(AnimationMontage))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Failed To Find Montage!"),
			true, false, FLinearColor::Red, DrawDebugDuration);
		bOutTraversalCheckFailed = false;
		bOutMontageSelectionFailed = true;
		return;
	}
	TraversalCheckResult.ChosenMontage = AnimationMontage;
	TraversalCheckResult.StartTime = Result.SelectedTime;
	TraversalCheckResult.PlayRate = Result.WantedPlayRate;

	TraversalResult = TraversalCheckResult;
	UpdateWarpTargets();
	PlayAnimationMontage(TraversalResult.ChosenMontage, TraversalResult.PlayRate, TraversalResult.StartTime);
	bDoingTraversalAction = true;
	GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, true);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	if (DrawDebugLevel >= 1)
	{
		UKismetSystemLibrary::PrintString(this, TraversalCheckResult.ToString(),
			true, false, FLinearColor(0.0f, 0.66f, 1.0f), DrawDebugDuration);
		UKismetSystemLibrary::PrintString(this, UEnum::GetValueAsString(TraversalCheckResult.ActionType),
			true, false, FLinearColor(1.0f, 0.0f, 0.824021f), DrawDebugDuration);
		const FString PerfString = FString::Printf(TEXT("Execution Time: %f seconds"),
			FPlatformTime::Seconds() - StartTime);
		UKismetSystemLibrary::PrintString(this, PerfString, true, true,
			FLinearColor(1.0f, 0.5f, 0.15f), DrawDebugDuration);
	}
}

float AGameAnimationBaseCharacter::GetTraversalForwardTraceDistance() const
{
	static const FVector2D RangeA = FVector2D(0.0f, 500.0f);
	static const FVector2D RangeB = FVector2D(75.0f, 350.0f);
	return FMath::GetMappedRangeValueClamped(RangeA, RangeB,
		GetActorRotation().Quaternion().UnrotateVector(GetCharacterMovement()->Velocity).X);
}

void AGameAnimationBaseCharacter::UpdateWarpTargets() const
{
	// Warp Target Name declarations
	static const FName FrontLedgeWarpTargetName = TEXT("FrontLedge");
	static const FName BackLedgeWarpTargetName = TEXT("BackLedge");
	static const FName BackFloorWarpTargetName = TEXT("BackFloor");
	static const FName DistanceFromLedgeCurveName = TEXT("Distance_From_Ledge");
	
	// Update the FrontLedge warp target using the front ledge's location and rotation.
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FrontLedgeWarpTargetName,
		TraversalResult.FrontLedgeLocation, FRotationMatrix::MakeFromX(-TraversalResult.FrontLedgeNormal).Rotator());

	float AnimatedDistanceFromFrontLedgeToBackLedge = 0.0f;
	// If the action type was a hurdle or a vault, we need to also update the BackLedge target. If it is not a hurdle or vault, remove it.
	if (TraversalResult.ActionType == ETraversalActionType::Hurdle ||
		TraversalResult.ActionType == ETraversalActionType::Vault)
	{
		// Because the traversal animations move at different distances (no fixed metrics), we need to know how far the animation moves in order to warp it properly. Here we cache a curve value at the end of the Back Ledge warp window to determine how far the animation is from the front ledge once the character reaches the back ledge location in the animation.
		TArray<FMotionWarpingWindowData> MotionWarpingWindowData; 
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(TraversalResult.ChosenMontage,
			BackLedgeWarpTargetName, MotionWarpingWindowData);
		if (!MotionWarpingWindowData.IsEmpty())
		{
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(TraversalResult.ChosenMontage,
				DistanceFromLedgeCurveName, MotionWarpingWindowData[0].EndTime,
				AnimatedDistanceFromFrontLedgeToBackLedge);

			// Update the BackLedge warp target.
			MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(BackLedgeWarpTargetName,
				TraversalResult.BackLedgeLocation, FRotator::ZeroRotator);
		}
		else
		{
			MotionWarping->RemoveWarpTarget(BackLedgeWarpTargetName);
		}
	}
	else
	{
		MotionWarping->RemoveWarpTarget(BackLedgeWarpTargetName);
	}

	// If the action type was a hurdle, we need to also update the BackFloor target. If it is not a hurdle, remove it.
	if (TraversalResult.ActionType == ETraversalActionType::Hurdle)
	{
		// Caches a curve value at the end of the Back Floor warp window to determine how far the animation is from the front ledge once the character touches the ground.
		TArray<FMotionWarpingWindowData> MotionWarpingWindowData; 
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(TraversalResult.ChosenMontage,
			BackFloorWarpTargetName, MotionWarpingWindowData);
		if (!MotionWarpingWindowData.IsEmpty())
		{
			float AnimatedDistanceFromFrontLedgeToBackFloor = 0.0f;
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(TraversalResult.ChosenMontage,
			                                                     DistanceFromLedgeCurveName, MotionWarpingWindowData[0].EndTime,
			                                                     AnimatedDistanceFromFrontLedgeToBackFloor);

			// Since the animations may land on the floor at different distances (a run hurdle may travel further than a walk or stand hurdle), use the total animated distance away from the back ledge as the X and Y values of the BackFloor warp point. This could technically cause some collision issues if the floor is not flat, or there is an obstacle in the way, therefore having fixed metrics for all traversal animations would be an improvement.
			const FVector Vector1 = TraversalResult.BackLedgeNormal *
				FMath::Abs(AnimatedDistanceFromFrontLedgeToBackLedge - AnimatedDistanceFromFrontLedgeToBackFloor);
			const FVector Vector2 = TraversalResult.BackLedgeLocation + Vector1;
			MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(BackFloorWarpTargetName,
				FVector(Vector2.X ,Vector2.Y, TraversalResult.BackFloorLocation.Z),
				FRotator::ZeroRotator);
		}
		else
		{
			MotionWarping->RemoveWarpTarget(BackFloorWarpTargetName);
		}
	}
	else
	{
		MotionWarping->RemoveWarpTarget(BackFloorWarpTargetName);
	}
}

void AGameAnimationBaseCharacter::PlayAnimationMontage_Implementation(const UAnimMontage* Montage, float PlayRate,
	float StartingPosition)
{
}

void AGameAnimationBaseCharacter::OnAnimationMontageCompletedOrInterrupted()
{
	bDoingTraversalAction = false;
	GetCapsuleComponent()->IgnoreComponentWhenMoving(TraversalResult.HitComponent, false);
	GetCharacterMovement()->SetMovementMode(TraversalResult.ActionType == ETraversalActionType::Vault ? MOVE_Falling : MOVE_Walking);
}

void AGameAnimationBaseCharacter::PlayAudioEvent_Implementation(const FGameplayTag& Value, float VolumeMultiplier,
	float PitchMultiplier)
{
}

void UPoseSearchTrajectoryLibraryExtension::HandleTrajectoryWorldCollisionsThreadSafe(const UObject* WorldContextObject, const UAnimInstance* AnimInstance, const FPoseSearchQueryTrajectory& InTrajectory, bool bApplyGravity, float FloorCollisionsOffset, FPoseSearchQueryTrajectory& OutTrajectory, FPoseSearchTrajectory_WorldCollisionResults& CollisionResult, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, float MaxObstacleHeight, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	UPoseSearchTrajectoryLibrary::HandleTrajectoryWorldCollisions(WorldContextObject, AnimInstance, InTrajectory,
		bApplyGravity, FloorCollisionsOffset, OutTrajectory, CollisionResult, TraceChannel, bTraceComplex,
		ActorsToIgnore, DrawDebugType, bIgnoreSelf, MaxObstacleHeight, TraceColor, TraceHitColor, DrawTime);
}
