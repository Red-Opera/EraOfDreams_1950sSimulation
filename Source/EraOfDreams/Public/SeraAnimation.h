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

    // 캐릭터 이동 속도 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float speed = 0.0f;

    // 캐릭터 공중 상태 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool isInAir = false;

    // 오른손 IK 활성화 상태 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    bool enableRightHandIK = false;

    // 오른손 IK 월드 위치 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    FVector rightHandIKLocation = FVector::ZeroVector;

    // 오른손 IK 가중치 변수 (0-1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    float rightHandIKAlpha = 0.0f;

    // 오른손 본의 로컬 위치 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    FVector rightHandLocalLocation = FVector::ZeroVector;

    // 왼손 IK 활성화 상태 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    bool enableLeftHandIK = false;

    // 왼손 IK 월드 위치 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    FVector leftHandIKLocation = FVector::ZeroVector;

    // 왼손 IK 가중치 변수 (0-1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    float leftHandIKAlpha = 0.0f;

    // 왼손 본의 로컬 위치 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand IK")
    FVector leftHandLocalLocation = FVector::ZeroVector;

    // 문 상호작용 상태 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Interaction")
    bool isInteractingWithDoor = false;

    // IK 전환 속도 설정 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Settings")
    float ikTransitionSpeed = 5.0f;

    // 최대 도달 거리 설정 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Settings")
    float maxReachDistance = 100.0f;

    // 오른손 IK 위치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    FVector GetRightHandIKLocation() const { return rightHandIKLocation; }

    // 오른손 IK 가중치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    float GetRightHandIKAlpha() const { return rightHandIKAlpha; }

    // 오른손 IK 활성화 상태 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    bool IsRightHandIKEnabled() const { return enableRightHandIK; }

    // 오른손 본의 로컬 위치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    FVector GetRightHandLocalLocation() const { return rightHandLocalLocation; }

    // 왼손 IK 위치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    FVector GetLeftHandIKLocation() const { return leftHandIKLocation; }

    // 왼손 IK 가중치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    float GetLeftHandIKAlpha() const { return leftHandIKAlpha; }

    // 왼손 IK 활성화 상태 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    bool IsLeftHandIKEnabled() const { return enableLeftHandIK; }

    // 왼손 본의 로컬 위치 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Hand IK", meta = (BlueprintThreadSafe = "true"))
    FVector GetLeftHandLocalLocation() const { return leftHandLocalLocation; }

    // 문 상호작용 상태 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Door Interaction", meta = (BlueprintThreadSafe = "true"))
    bool IsInteractingWithDoor() const { return isInteractingWithDoor; }

    // 손과 손잡이 사이 거리 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Door Interaction", meta = (BlueprintThreadSafe = "true"))
    float GetHandToHandleDistance() const;

public:
    // 문 상호작용 시작 함수
    UFUNCTION(BlueprintCallable, Category = "Door Interaction")
    void OnDoorInteractionStarted(const FVector& handleLocation, class ADoor* interactingDoor);

protected:
    // 캐릭터 참조
    UPROPERTY(BlueprintReadOnly, Category = "Character")
    class ASera* seraCharacter;

    // 캐릭터 무브먼트 컴포넌트 참조
    UPROPERTY(BlueprintReadWrite, Category = "Character")
    class UCharacterMovementComponent* characterMovement;

private:
    void UpdateDoorInteractionValues();         // 문 상호작용 상태 업데이트 함수
    void UpdateRightHandIK(float DeltaTime);    // 오른손 IK 업데이트 함수
    
    // 현재 문의 손잡이 위치 반환 함수
    FVector GetDoorLeftPositionHandleLocation();
    
    // 가장 가까운 문 객체 찾기 함수
    class ADoor* GetNearestDoor();
    
    // 타겟이 도달 가능한 범위 내에 있는지 확인 함수
    bool IsTargetWithinReach(const FVector& targetLocation);
    
    // 월드 위치를 본 로컬 위치로 변환하는 함수
    FVector ConvertWorldToLocalBoneSpace(const FVector& worldLocation, const FName& boneName);
    
    FVector targetRightHandIKLocation = FVector::ZeroVector;    // 목표 IK 위치 변수
    FVector previousRightHandIKLocation = FVector::ZeroVector;  // 이전 IK 위치 변수
    
    // 문 상호작용 종료 전환 상태 변수
    bool isTransitioningFromDoor = false;
    
    // 마지막 유효 손 위치 변수
    FVector lastValidHandPosition = FVector::ZeroVector;

    float transitionTimer = 0.0f;       // 전환 타이머 변수
    float transitionDuration = 0.5f;    // 전환 지속 시간 변수

    // 현재 상호작용 중인 문 참조
    UPROPERTY()
    class ADoor* activeDoor = nullptr;
    
    // 손 IK가 목표에 도달 상태 변수
    bool hasReachedTarget = false;
    
    // 손 IK 완료 임계값 설정
    UPROPERTY(EditAnywhere, Category = "IK Settings")
    float handIKThreshold = 0.9f;

    // 손과 손잡이 사이 거리 저장 변수
    float currentHandToHandleDistance = 0.0f;

    // 소켓 이름 상수 추가
    const FName RightHandSocketName = FName("index_metacarpal_r");
    const FName LeftHandSocketName = FName("index_metacarpal_l");
    
    // 왼손 변수 추가
    FVector targetLeftHandIKLocation = FVector::ZeroVector;
    FVector previousLeftHandIKLocation = FVector::ZeroVector;
    FVector lastValidLeftHandPosition = FVector::ZeroVector;
    float currentLeftHandToHandleDistance = 0.0f;
};