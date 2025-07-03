#include "Sera.h"
#include "SeraAnimation.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Core/GameAnimationBasePlayerController.h"

ASera::ASera()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 기본적으로 플레이어 컨트롤 활성화
    isControlMovement = true;
    
    // 문 상호작용 변수 초기화
    isDoorInteraction = false;
    isPaused = false;
    doorInteractionSpeed = 0.0f;
    doorInteractionSideSpeed = 0.0f;
}

void ASera::BeginPlay()
{
    Super::BeginPlay();
}

void ASera::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    
    //// 문 상호작용 중 자동 이동 처리
    if (isDoorInteraction && !isPaused)
    {
        // GameAnimationBasePlayerController 사용 방식으로 이동 처리
        APlayerController* PC = Cast<APlayerController>(GetController());
    
        if (PC)
        {
            FVector direction = GetActorForwardVector() * doorInteractionSpeed + 
                               GetActorRightVector() * doorInteractionSideSpeed;
            AddMovementInput(direction, 1.0f);
        }
    }
}

void ASera::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
    Super::SetupPlayerInputComponent(playerInputComponent);
    // 입력 처리는 GameAnimationBasePlayerController에서 담당
}

void ASera::StartDoorInteractionMovement(float speed, float sideMovement)
{
    // 첫 상호작용 시작 시에만 이전 상태 저장
    if (!isDoorInteraction)
    {
        // 플레이어 컨트롤 비활성화
        isControlMovement = false;
        
        // 이전 걷기/달리기 상태 저장
        wasRunning = false;
        
        // 강제 걷기 모드 설정
        if (AGameAnimationBasePlayerController* PC = Cast<AGameAnimationBasePlayerController>(GetController()))
        {
            if (PC->GetGameAnimationBaseCharacter())
            {
                // 이전 상태 저장
                wasRunning = PC->GetGameAnimationBaseCharacter()->bWantsToSprint;
                
                // 걷기 모드 강제 설정
                PC->GetGameAnimationBaseCharacter()->bWantsToSprint = false;
                PC->GetGameAnimationBaseCharacter()->bWantsToWalk = true;

                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
                    FString::Printf(TEXT("문 통과 시작: %s 모드로 전환됨"), 
						PC->GetGameAnimationBaseCharacter()->bWantsToSprint ? TEXT("달리기") : TEXT("걷기")));   
            }

            else
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
					TEXT("문 통과 오류: GameAnimationBaseCharacter 참조가 없습니다"));
        }
    
        // 상호작용 시작 로그
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
            TEXT("Door interaction movement started - Player control disabled"));
    }
    
    // 상호작용 상태 설정
    isDoorInteraction = true;
    
    // 속도에 따른 일시정지 상태 설정
    isPaused = FMath::IsNearlyZero(speed) && FMath::IsNearlyZero(sideMovement);
    
    // 속도 범위 제한 적용 (더 낮은 최대값으로 제한)
    doorInteractionSpeed = FMath::Clamp(speed, -0.4f, 0.4f);
    doorInteractionSideSpeed = FMath::Clamp(sideMovement, -0.4f, 0.4f);
}

void ASera::StopDoorInteractionMovement()
{
    // 상호작용 상태 해제
    isDoorInteraction = false;
    
    // 이동 속도 초기화
    doorInteractionSpeed = 0.0f;
    doorInteractionSideSpeed = 0.0f;
    
    // 플레이어 컨트롤 다시 활성화
    isControlMovement = true;
    
    // 이전 달리기/걷기 상태 복원
    if (AGameAnimationBasePlayerController* PC = Cast<AGameAnimationBasePlayerController>(GetController()))
    {
        if (PC->GetGameAnimationBaseCharacter())
        {
            // 원래 상태로 복원
            PC->GetGameAnimationBaseCharacter()->bWantsToSprint = wasRunning;
            PC->GetGameAnimationBaseCharacter()->bWantsToWalk = !wasRunning;
            
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
                FString::Printf(TEXT("문 통과 완료: 이전 %s 모드로 복원"), 
                wasRunning ? TEXT("달리기") : TEXT("걷기")));
        }
    }
    
    // 상호작용 종료 로그
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
        TEXT("Door interaction movement stopped - Player control restored"));
}