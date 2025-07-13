// Copyright (c) 2024 Marvin Bernd Watanabe

#pragma once

#include "GameAnimationBaseCoreTypes.h"
#include "GameAnimationBasePlayerControllerInterface.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "GameAnimationBasePlayerController.generated.h"

#define REGISTER_INPUT_ACTION(EnhancedInputComponent, InputAction, TriggerEvent) if (InputAction.IsNull()) { GAB_LOG(LogGameAnimationBase, Error, TEXT("An action input property is null!")); } else { EnhancedInputComponent->BindAction(InputAction.LoadSynchronous(), TriggerEvent, this, &AGameAnimationBasePlayerController::On##InputAction); }

UCLASS(Abstract)
class ERAOFDREAMS_API AGameAnimationBasePlayerController : public APlayerController, public IGameAnimationBasePlayerControllerInterface
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character")
		TObjectPtr<class AGameAnimationBaseCharacter> GameAnimationBaseCharacter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Input")
		TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
		TSoftObjectPtr<UInputMappingContext> InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
		int32 InputMappingPriority = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
		EAnalogMovementBehavior MovementStickMode = EAnalogMovementBehavior::FixedSpeedSingleGait;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
		float AnalogWalkRunThreshold = 0.7;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> MoveInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> MoveWorldSpaceInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> LookInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> LookGamepadInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> SprintInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> WalkInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> JumpInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> CrouchInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> StrafeInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> PerspectiveInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input|Action")
		TSoftObjectPtr<UInputAction> AimInputAction;

	// This function normalizes the input scale when the movement stick mode is set to fixed speed. This means any analog input from a gamepad will be converted to max input, similar to the behavior of keyboard movement. Improvements will be made to better support variable speeds in the future.
	UFUNCTION(BlueprintPure, Category="Input")
		virtual FVector2D GetMovementInputScaleValue(const FVector2D& Input) const;
	UFUNCTION(BlueprintCallable, Category="Input|Action")
		virtual FVector2D GetMoveInputActionValue() const;
	UFUNCTION(BlueprintCallable, Category="Input|Action")
		virtual FVector2D GetMoveWorldSpaceInputActionValue() const;

	UFUNCTION()
		virtual void OnMoveInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnMoveWorldSpaceInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnLookInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnLookGamepadInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnSprintInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnWalkInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnJumpInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnCrouchInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnStrafeInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnPerspectiveInputAction(const FInputActionInstance& Instance);
	UFUNCTION()
		virtual void OnAimInputAction(const FInputActionInstance& Instance);

public:
	virtual bool IsFullMovementInput_Implementation() const;

	FORCEINLINE virtual AGameAnimationBaseCharacter* GetGameAnimationBaseCharacter() const { return GameAnimationBaseCharacter; }
};
