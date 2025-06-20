#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"
#include "SeraAnimation.generated.h"

UCLASS()
class ERAOFDREAMS_API USeraAnimation : public UAnimInstance
{
    GENERATED_BODY()

public:
    USeraAnimation();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

    // 기본 애니메이션 변수들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float speed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool isInAir = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool isMoving = false;

    // 오른손 IK 관련 변수들 (LeftPositionHandle을 위한)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    bool enableRightHandIK = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    FVector rightHandIKLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    float rightHandIKAlpha = 0.0f;

    // 오른손 본의 로컬 위치 (Control Rig용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    FVector rightHandLocalLocation = FVector::ZeroVector;

    // 문 상호작용 관련
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Interaction")
    bool isInteractingWithDoor = false;

    // IK 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Settings")
    float ikTransitionSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Settings")
    float maxReachDistance = 100.0f;

    // Blueprint에서 호출 가능한 스레드 안전 함수들
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    FVector GetRightHandIKLocation() const;

    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    float GetRightHandIKAlpha() const { return rightHandIKAlpha; }

    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    bool IsRightHandIKEnabled() const { return enableRightHandIK; }

    // 오른손 본의 로컬 위치를 반환하는 함수 추가
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    FVector GetRightHandLocalLocation() const { return rightHandLocalLocation; }

    UFUNCTION(BlueprintCallable, Category = "Door Interaction", meta = (BlueprintThreadSafe = "true"))
    bool IsInteractingWithDoor() const { return isInteractingWithDoor; }

public:
    // 문 상호작용 시작을 알리는 함수 추가
    UFUNCTION(BlueprintCallable, Category = "Door Interaction")
    void OnDoorInteractionStarted(const FVector& HandleLocation);

private:
    // DoorHandleLocation 변수 제거 (사용되지 않음)
    
protected:
    // 캐릭터 참조
    UPROPERTY(BlueprintReadOnly, Category = "Character")
    class ASera* seraCharacter;

    // 캐릭터 무브먼트 컴포넌트 참조
    UPROPERTY(BlueprintReadOnly, Category = "Character")
    class UCharacterMovementComponent* characterMovement;

private:
    // 애니메이션 업데이트 함수들
    void UpdateMovementValues();
    void UpdateDoorInteractionValues();
    void UpdateRightHandIK(float DeltaTime);
    
    // 현재 상호작용 중인 문의 LeftPositionHandle 위치를 가져오는 함수
    FVector GetDoorLeftPositionHandleLocation();
    
    // 가장 가까운 문을 찾는 함수 추가
    class ADoor* GetNearestDoor();
    
    // 타겟이 도달 가능한 범위 내에 있는지 확인
    bool IsTargetWithinReach(const FVector& TargetLocation);
    
    // 월드 위치를 캐릭터 메시의 로컬 위치로 변환하는 함수
    FVector ConvertWorldToLocalBoneSpace(const FVector& WorldLocation, const FName& BoneName);
};