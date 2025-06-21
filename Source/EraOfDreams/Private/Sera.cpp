#include "Sera.h"
#include "SeraAnimation.h"
#include "KeyInputManager.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

ASera::ASera()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASera::BeginPlay()
{
    Super::BeginPlay();

    // 달리기 키 이벤트 등록
    KeyInputManager::AddEvent(EKeys::LeftShift, this, &ASera::SetIsRunTrue, EKeyEventType::Pressed);
    KeyInputManager::AddEvent(EKeys::LeftShift, this, &ASera::SetIsRunFalse, EKeyEventType::Released);
}

void ASera::MoveForward(float value)
{
    forwardAxisSpeed = value;

    // 카메라 방향 기준 전방 벡터 계산
    float rotation = GetControlRotation().Yaw;
    FRotator rotationValue = FRotator(0.0f, rotation, 0.0f);

    // 이동 방향 및 속도 계산
    FVector forwardVector = UKismetMathLibrary::GetForwardVector(rotationValue);
    float forwardSpeed = GetForwardSpeed();

    // 캐릭터 이동 적용
    AddMovementInput(forwardVector, forwardSpeed);
}

void ASera::MoveRight(float value)
{
    rightAxisSpeed = value;

    // 카메라 방향 기준 측면 벡터 계산
    float rotation = GetControlRotation().Yaw;
    FRotator rotationValue = FRotator(0.0f, rotation, 0.0f);

    // 이동 방향 및 속도 계산
    FVector rightVector = UKismetMathLibrary::GetRightVector(rotationValue);
    float rightSpeed = GetRightSpeed();

    // 캐릭터 이동 적용
    AddMovementInput(rightVector, rightSpeed);
}

void ASera::StartRun()
{
    isRun = true;
}

void ASera::StopRun()
{
    isRun = false;
}

void ASera::SetIsRunTrue()
{
    isRun = true;
}

void ASera::SetIsRunFalse()
{
    isRun = false;
}

void ASera::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    
    // 문 상호작용 중 자동 이동 처리
    if (isDoorInteraction && !isPaused)
        MoveForward(doorInteractionSpeed);
}

void ASera::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
    Super::SetupPlayerInputComponent(playerInputComponent);
}

bool ASera::GetIsRun()
{
    return isRun;
}

float ASera::GetForwardSpeed() const
{
    // 달리기 상태일 때 정상 속도, 아닐 때 감소된 속도
    if (forwardAxisSpeed >= 0.0 && isRun)
        return forwardAxisSpeed;

    return forwardAxisSpeed / 3.0f;
}

float ASera::GetRightSpeed() const
{
    // 달리기 상태일 때 정상 속도, 아닐 때 감소된 속도
    if (forwardAxisSpeed >= 0.0 && isRun)
        return rightAxisSpeed;

    return rightAxisSpeed / 3.0f;
}

void ASera::StartDoorInteractionMovement(float speed)
{
    // 첫 상호작용 시작 시에만 이전 속도 저장
    if (!isDoorInteraction)
    {
        previousForwardSpeed = forwardAxisSpeed;

        // 상호작용 시작 로그
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
                TEXT("Door interaction movement started"));
        }
    }

    // 상호작용 상태 설정
    isDoorInteraction = true;

    // 속도에 따른 일시정지 상태 설정
    isPaused = FMath::IsNearlyZero(speed);

    // 속도 범위 제한 적용
    doorInteractionSpeed = FMath::Clamp(speed, 0.0f, 2.0f);

    // 현재 상태 로그
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan,
        FString::Printf(TEXT("Door interaction speed: %.2f, %s"),
            doorInteractionSpeed, isPaused ? TEXT("일시정지") : TEXT("이동 중")));
}

void ASera::StopDoorInteractionMovement()
{
    // 상호작용 상태 해제
    isDoorInteraction = false;

    // 이동 속도 초기화
    doorInteractionSpeed = 0.0f;

    // 원래 속도로 복원
    MoveForward(previousForwardSpeed);

    // 상호작용 종료 로그
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
        TEXT("Door interaction movement stopped"));
}