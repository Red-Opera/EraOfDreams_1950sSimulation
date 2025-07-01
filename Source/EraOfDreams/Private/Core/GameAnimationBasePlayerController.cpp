// Copyright (c) 2024 Marvin Bernd Watanabe

#include "Core/GameAnimationBasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/GameAnimationBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/NavMovementComponent.h"

void AGameAnimationBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);
	
	REGISTER_INPUT_ACTION(EnhancedInputComponent, MoveInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, MoveWorldSpaceInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, LookInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, LookGamepadInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, SprintInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, WalkInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, JumpInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, CrouchInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, StrafeInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, PerspectiveInputAction, ETriggerEvent::Triggered);
	REGISTER_INPUT_ACTION(EnhancedInputComponent, AimInputAction, ETriggerEvent::Triggered);
}

void AGameAnimationBasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InputMapping.IsNull())
	{
		GAB_LOG(LogGameAnimationBase, Error, TEXT("The InputMapping property is null!"));
		return;
	}

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);
	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystem);
	InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), InputMappingPriority);

	GameAnimationBaseCharacter = Cast<AGameAnimationBaseCharacter>(InPawn);
}

FVector2D AGameAnimationBasePlayerController::GetMovementInputScaleValue(const FVector2D& Input) const
{
	switch (MovementStickMode)
	{
	default:
	case EAnalogMovementBehavior::FixedSpeedSingleGait:
	case EAnalogMovementBehavior::FixedSpeedWalkRun:
		return Input.GetSafeNormal();
	case EAnalogMovementBehavior::VariableSpeedSingleGait:
	case EAnalogMovementBehavior::VariableSpeedWalkRun:
		return Input;
	}
}

FVector2D AGameAnimationBasePlayerController::GetMoveInputActionValue() const
{
	if (IsValid(EnhancedInputComponent))
	{
		if (!MoveInputAction.IsNull())
		{
			return EnhancedInputComponent->BindActionValue(MoveInputAction.LoadSynchronous()).GetValue().Get<FVector2D>(); 
		}
	}
	return FVector2D::ZeroVector;
}

FVector2D AGameAnimationBasePlayerController::GetMoveWorldSpaceInputActionValue() const
{
	if (IsValid(EnhancedInputComponent))
	{
		if (!MoveWorldSpaceInputAction.IsNull())
		{
			return EnhancedInputComponent->BindActionValue(MoveWorldSpaceInputAction.LoadSynchronous()).GetValue().Get<FVector2D>(); 
		}
	}
	return FVector2D::ZeroVector;
}

bool AGameAnimationBasePlayerController::IsFullMovementInput_Implementation() const
{
	return MovementStickMode == EAnalogMovementBehavior::FixedSpeedSingleGait ||
		MovementStickMode == EAnalogMovementBehavior::VariableSpeedSingleGait ||
			((MovementStickMode == EAnalogMovementBehavior::FixedSpeedWalkRun ||
				MovementStickMode == EAnalogMovementBehavior::VariableSpeedWalkRun) &&
				(GetMoveInputActionValue().Length() >= AnalogWalkRunThreshold ||
					GetMoveWorldSpaceInputActionValue().Length() >= AnalogWalkRunThreshold));
}

void AGameAnimationBasePlayerController::OnMoveInputAction(const FInputActionInstance& Instance)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InputScale = GetMovementInputScaleValue(Instance.GetValue().Get<FVector2D>());
		const FRotator Rotator = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		ControlledPawn->AddMovementInput(FRotationMatrix(Rotator).GetScaledAxis(EAxis::Y), InputScale.X);
		ControlledPawn->AddMovementInput(Rotator.Vector(), InputScale.Y);
	}
}

void AGameAnimationBasePlayerController::OnMoveWorldSpaceInputAction(const FInputActionInstance& Instance)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InputScale = Instance.GetValue().Get<FVector2D>().GetSafeNormal();
		ControlledPawn->AddMovementInput(FVector::RightVector, InputScale.X);
		ControlledPawn->AddMovementInput(FVector::ForwardVector, InputScale.Y);
	}
}

void AGameAnimationBasePlayerController::OnLookInputAction(const FInputActionInstance& Instance)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InputScale = Instance.GetValue().Get<FVector2D>();
		ControlledPawn->AddControllerYawInput(InputScale.X);
		ControlledPawn->AddControllerPitchInput(InputScale.Y);
	}
}

void AGameAnimationBasePlayerController::OnLookGamepadInputAction(const FInputActionInstance& Instance)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InputScale = Instance.GetValue().Get<FVector2D>() * GetWorld()->GetDeltaSeconds();
		ControlledPawn->AddControllerYawInput(InputScale.X);
		ControlledPawn->AddControllerPitchInput(InputScale.Y);
	}
}

void AGameAnimationBasePlayerController::OnSprintInputAction(const FInputActionInstance& Instance)
{
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		// Sprint Input (Coming in 5.5)
		ControlledPawn->bWantsToSprint = Instance.GetValue().Get<bool>();
		ControlledPawn->bWantsToWalk = false;
	}
}

void AGameAnimationBasePlayerController::OnWalkInputAction(const FInputActionInstance& Instance)
{
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		if (!ControlledPawn->bWantsToSprint)
		{
			ControlledPawn->bWantsToWalk = !ControlledPawn->bWantsToWalk;
		}
	}
}

void AGameAnimationBasePlayerController::OnJumpInputAction(const FInputActionInstance& Instance)
{
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		if (!ControlledPawn->bDoingTraversalAction)
		{
			if (const UNavMovementComponent* NavCharacterMovementComponent =
				Cast<UNavMovementComponent>(ControlledPawn->GetCharacterMovement()))
			{
				if (NavCharacterMovementComponent->IsMovingOnGround())
				{
					bool bTraversalCheckFailed, bMontageSelectionFailed;
					ControlledPawn->TryTraversalAction(ControlledPawn->GetTraversalForwardTraceDistance(),
						bTraversalCheckFailed, bMontageSelectionFailed);
					if (bTraversalCheckFailed)
					{
						ControlledPawn->Jump();
					}
				}
			}
		}
	}
}

void AGameAnimationBasePlayerController::OnCrouchInputAction(const FInputActionInstance& Instance)
{
	// Crouch Input (Coming in 5.5)
#if false
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		if (ControlledPawn->bIsCrouched)
		{
			ControlledPawn->UnCrouch();
		}
		else
		{
			ControlledPawn->Crouch();
		}
	}
#endif
}

void AGameAnimationBasePlayerController::OnStrafeInputAction(const FInputActionInstance& Instance)
{
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		ControlledPawn->bWantsToStrafe = !ControlledPawn->bWantsToStrafe;
	}
}

void AGameAnimationBasePlayerController::OnPerspectiveInputAction(const FInputActionInstance& Instance)
{
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		ControlledPawn->bWantsFirstPerson = !ControlledPawn->bWantsFirstPerson;
	}
}

void AGameAnimationBasePlayerController::OnAimInputAction(const FInputActionInstance& Instance)
{
	if (AGameAnimationBaseCharacter* ControlledPawn = GetGameAnimationBaseCharacter())
	{
		ControlledPawn->bWantsToAim = Instance.GetValue().Get<bool>();
		if (ControlledPawn->bWantsToAim)
		{
			ControlledPawn->bWantsToStrafe = true;
		}
	}
}
