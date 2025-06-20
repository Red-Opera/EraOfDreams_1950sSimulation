#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Door.generated.h"

UCLASS()
class ERAOFDREAMS_API ADoor : public AActor
{
    GENERATED_BODY()
    
public:
    ADoor();

    virtual void Tick(float DeltaTime) override;

    // Blueprint에서 설정 가능한 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* doorMesh;

    // 상호작용 범위를 위한 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* interactionBox;

    // Blueprint에서 설정 가능한 문 회전 각도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    float openAngle = 90.0f;

    // 문 열림 / 닫힘 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    float doorSpeed = 1.5f;

    // 문 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isOpen = false;

    // 문이 열리는 중인지 확인
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isMoving = false;
    
    // 문 상호작용 준비 상태 (손 IK가 적용 중이지만 아직 문이 움직이지 않음)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isPreparing = false;
    
    // 상호작용 박스 크기 조절
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    FVector interactionExtent = FVector(150.0f, 150.0f, 100.0f);
    
    // 상호작용 박스 오프셋 (위치 조절)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    FVector interactionOffset = FVector(0.0f, 0.0f, 0.0f);

    // LeftPositionHandle의 월드 위치를 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    FVector GetLeftPositionHandleWorldLocation() const;

    // 문이 움직이고 있는지 확인하는 함수 추가
    UFUNCTION(BlueprintCallable, Category = "Door")
    bool IsMoving() const { return isMoving; }
    
    // 문이 준비 중인지 확인하는 함수 추가
    UFUNCTION(BlueprintCallable, Category = "Door")
    bool IsPreparing() const { return isPreparing; }
    
    // 플레이어 상호작용 수동으로 테스트하는 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    void TryToggleDoor();
    
    // 실제 문 움직임 시작 (손 IK가 완료된 후 호출됨)
    UFUNCTION(BlueprintCallable, Category = "Door")
    void StartDoorMovement();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 상호작용 박스 오버랩 이벤트
    UFUNCTION()
    void OnInteractionBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, 
        UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& sweepResult);

    UFUNCTION()
    void OnInteractionEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, 
        UPrimitiveComponent* otherComp, int32 otherBodyIndex);

private:
    // 문 토글 준비 함수 (손 IK 시작)
    void ToggleDoor();

    // 플레이어가 상호작용 범위에 있는지 확인
    bool isPlayerInRange = false;

    // 문의 초기 회전값과 목표 회전값
    FRotator initialRotation;
    FRotator targetRotation;
    FRotator currentRotation;

    // 플레이어 참조
    class ASera* playerCharacter;
    
    // 디버그 타이머
    float debugTimeSinceLastCheck = 0.0f;
    
    // 문을 열 것인지 닫을 것인지 결정하는 플래그 (준비 중일 때 사용)
    bool shouldOpen = false;
};