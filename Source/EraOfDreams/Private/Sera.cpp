#include "Sera.h"
#include "SeraAnimation.h"
#include "KeyInputManager.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

ASera::ASera()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASera::BeginPlay()
{
    Super::BeginPlay();

    // 키 누를 때와 뗄 때 이벤트 등록 (개선된 방식)
    KeyInputManager::AddEvent(EKeys::LeftShift, this, &ASera::SetIsRunTrue, EKeyEventType::Pressed);
    KeyInputManager::AddEvent(EKeys::LeftShift, this, &ASera::SetIsRunFalse, EKeyEventType::Released);
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
	// 앞으로 이동하고 있고, 달리기 상태인 경우
    if (forwardAxisSpeed >= 0.0 && isRun)
        return forwardAxisSpeed;

    return forwardAxisSpeed / 3.0f;
}

float ASera::GetRightSpeed() const
{
    if (forwardAxisSpeed >= 0.0 && isRun)
		return rightAxisSpeed;

	return rightAxisSpeed / 3.0f;
}
