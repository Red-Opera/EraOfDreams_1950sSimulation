#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreaming.h"
#include "Door.generated.h"

// 레벨 전환 완료 시 호출할 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorLevelTransitionEvent);

UCLASS()
class ERAOFDREAMS_API ADoor : public AActor
{
    GENERATED_BODY()
    
public:
    ADoor();

    virtual void Tick(float DeltaTime) override;

    // 문의 기본 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* doorMesh;

    // 플레이어 상호작용 감지용 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* interactionBox;

    // 문이 열릴 때 회전할 각도(도)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    float openAngle = 90.0f;

    // 문 열림/닫힘 속도 조절값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    float doorSpeed = 1.5f;

    // 레벨 전환 관련 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
    bool shouldLoadNewLevel = false;
    
    // Blueprint에서 직접 레벨 에셋 선택 가능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition", meta = (EditCondition = "shouldLoadNewLevel", AllowedClasses = "World"))
    TSoftObjectPtr<UWorld> levelToLoadAsset;
    
    // 문이 완전히 열렸을 때 레벨 로드 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition", meta = (EditCondition = "shouldLoadNewLevel"))
    bool loadLevelWhenFullyOpen = true;

    // 레벨 로드 전 지연 시간(초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition", meta = (EditCondition = "shouldLoadNewLevel"))
    float delayBeforeLoading = 0.5f;
    
    // Blueprint에서 레벨 전환 처리 활성화 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition", meta = (EditCondition = "shouldLoadNewLevel"))
    bool useBlueprintLevelTransition = false;
    
    // 레벨 전환 시작 시 호출되는 델리게이트 (Blueprint에서 활용)
    UPROPERTY(BlueprintAssignable, Category = "Level Transition")
    FOnDoorLevelTransitionEvent OnLevelTransitionStarted;
    
    // Blueprint에서 호출할 수 있는 레벨 전환 함수
    UFUNCTION(BlueprintCallable, Category = "Level Transition")
    void LoadTargetLevel();

    // 문의 현재 열림 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isOpen = false;

    // 문이 현재 회전 중인지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isMoving = false;
    
    // 손 IK 적용 중 상태 (문 움직임 전 준비 단계)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isPreparing = false;
    
    // 상호작용 감지 영역의 크기 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    FVector interactionExtent = FVector(150.0f, 150.0f, 100.0f);
    
    // 상호작용 감지 영역의 위치 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    FVector interactionOffset = FVector(0.0f, 0.0f, 0.0f);

    // 왼쪽 문 손잡이 위치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    FVector GetLeftPositionHandleWorldLocation() const;

    // 문 회전 중 여부 확인 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    bool IsMoving() const { return isMoving; }
    
    // 문 준비 상태(손 IK 적용 중) 확인 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    bool IsPreparing() const { return isPreparing; }
    
    // 테스트용 수동 문 상호작용 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    void TryToggleDoor();
    
    // 손 IK 완료 후 실제 문 회전 시작 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    void StartDoorMovement();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 플레이어가 상호작용 영역에 들어올 때 호출
    UFUNCTION()
    void OnInteractionBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, 
        UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& sweepResult);

    // 플레이어가 상호작용 영역에서 나갈 때 호출
    UFUNCTION()
    void OnInteractionEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, 
        UPrimitiveComponent* otherComp, int32 otherBodyIndex);

private:
    // 손 IK 시작 및 문 상호작용 준비 함수
    void ToggleDoor();
    
    // 지정된 지연 후 레벨 전환 시작
    void StartLevelTransition();
    
    // 레벨 전환 중인지 여부
    bool isTransitioningLevel = false;

    // 플레이어가 상호작용 가능 범위 내에 있는지 여부
    bool isPlayerInRange = false;

    FRotator initialRotation;   // 문의 초기 회전값
    FRotator targetRotation;    // 문의 목표 회전값
    FRotator currentRotation;   // 문의 현재 회전값

    // 플레이어 캐릭터 참조
    class ASera* playerCharacter;
    
    // 디버그 정보 출력용 타이머
    float debugTimeSinceLastCheck = 0.0f;
    
    // 문 열기/닫기 상태 결정 플래그
    bool shouldOpen = false;
};