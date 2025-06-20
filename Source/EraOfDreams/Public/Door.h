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
    float doorSpeed = 2.0f;

    // 문 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isOpen = false;

    // 문이 열리는 중인지 확인
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Settings")
    bool isMoving = false;

    // LeftPositionHandle의 월드 위치를 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Door")
    FVector GetLeftPositionHandleWorldLocation() const;

    // 문이 움직이고 있는지 확인하는 함수 추가
    UFUNCTION(BlueprintCallable, Category = "Door")
    bool IsMoving() const { return isMoving; }

protected:
    virtual void BeginPlay() override;

    // 상호작용 박스 오버랩 이벤트
    UFUNCTION()
    void OnInteractionBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, 
        UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& sweepResult);

    UFUNCTION()
    void OnInteractionEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, 
        UPrimitiveComponent* otherComp, int32 otherBodyIndex);

private:
    // 문 토글 함수
    void ToggleDoor();

    // 플레이어가 상호작용 범위에 있는지 확인
    bool isPlayerInRange = false;

    // 문의 초기 회전값과 목표 회전값
    FRotator initialRotation;
    FRotator targetRotation;
    FRotator currentRotation;

    // 플레이어 참조
    class ASera* playerCharacter;
};