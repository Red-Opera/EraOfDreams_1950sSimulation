#include "Sera.h"
#include "SeraAnimation.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "MoveParkoru/Core/GameAnimationBasePlayerController.h"
#include "UI/PlayerMenu.h"
#include "KeyInputManager.h"
#include "Blueprint/UserWidget.h"

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
    
    // 초기화
    playerMenuClass = nullptr;
    createdPlayerMenu = nullptr;
}

void ASera::BeginPlay()
{
    Super::BeginPlay();
    
    // 인벤토리 토글을 위한 키 이벤트 등록
    KeyInputManager::AddEvent(EKeys::Tab, this, &ASera::HandleInventoryToggle, EKeyEventType::Pressed);
    KeyInputManager::AddEvent(EKeys::E, this, &ASera::HandleInventoryToggle, EKeyEventType::Pressed);
    
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Sera: 인벤토리 키 이벤트 등록 완료"));
    
    // 블루프린트에서 PlayerMenu 클래스가 설정되었다면 자동으로 생성
    if (playerMenuClass != nullptr)
    {
        CreatePlayerMenu();
    }
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

// PlayerMenu 위젯 인스턴스 반환 함수
UPlayerMenu* ASera::GetPlayerMenu() const
{
    return createdPlayerMenu;
}

// PlayerMenu 위젯 생성 함수
void ASera::CreatePlayerMenu()
{
    if (playerMenuClass != nullptr)
    {
        // 이미 생성된 위젯이 있으면 제거
        if (createdPlayerMenu != nullptr)
        {
            createdPlayerMenu->RemoveFromParent();
            createdPlayerMenu = nullptr;
        }
        
        // 플레이어 컨트롤러 가져오기
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            // PlayerMenu 위젯 생성
            createdPlayerMenu = CreateWidget<UPlayerMenu>(PC, playerMenuClass);
            
            if (createdPlayerMenu != nullptr)
            {
                // 뷰포트에 추가하되 숨김 상태로 시작
                createdPlayerMenu->AddToViewport();
                createdPlayerMenu->SetVisibility(ESlateVisibility::Hidden);
                
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, 
                    TEXT("PlayerMenu 위젯이 숨김 상태로 생성되었습니다."));
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
                    TEXT("PlayerMenu 위젯 생성에 실패했습니다."));
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
                TEXT("PlayerController를 찾을 수 없습니다."));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, 
            TEXT("PlayerMenu 클래스가 설정되지 않았습니다. 블루프린트에서 Player Menu Class를 설정해주세요."));
    }
}

// 인벤토리 토글 함수를 메뉴 토글로 변경
void ASera::ToggleInventory()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Sera: ToggleInventory 호출됨"));
    
    if (createdPlayerMenu != nullptr)
    {
        // 메뉴가 보이지 않는 상태라면 표시
        if (createdPlayerMenu->GetVisibility() == ESlateVisibility::Hidden)
        {
            createdPlayerMenu->SetVisibility(ESlateVisibility::Visible);
            
            // 메뉴가 표시될 때 인벤토리도 표시
            if (UInventory* inventory = createdPlayerMenu->GetInventory())
            {
                inventory->ShowInventory();
                GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("인벤토리 표시됨"));
            }
            
            // UI 모드 설정 및 마우스 커서 표시
            if (GetWorld() && GetWorld()->GetFirstPlayerController())
            {
                FInputModeGameAndUI InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                InputMode.SetHideCursorDuringCapture(false);
                
                GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
                GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
            }
            
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("메뉴 표시됨"));
        }
        // 메뉴가 보이는 상태라면 숨김
        else
        {
            createdPlayerMenu->SetVisibility(ESlateVisibility::Hidden);
            
            // 게임 전용 모드로 복원 및 마우스 커서 숨김
            if (GetWorld() && GetWorld()->GetFirstPlayerController())
            {
                GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
                GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
            }
            
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("메뉴 숨겨짐"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("PlayerMenu가 생성되지 않았습니다."));
        
        // PlayerMenu가 없으면 자동으로 생성 시도
        if (playerMenuClass != nullptr)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("PlayerMenu 자동 생성 시도"));
            CreatePlayerMenu();
        }
    }
}

// HandleInventoryToggle 함수를 다음과 같이 수정:
void ASera::HandleInventoryToggle()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Sera: HandleInventoryToggle 호출됨"));
    
    // 문과 상호작용 중이 아닐 때만 인벤토리 토글 가능
    if (!isDoorInteraction)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Sera: 문 상호작용 중이 아님, 인벤토리 토글 시도"));
        ToggleInventory();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("문 상호작용 중에는 인벤토리를 열 수 없습니다."));
    }
}

// 기존 문 상호작용 함수들은 그대로 유지...

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