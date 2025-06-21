#include "SeraAnimation.h"
#include "Sera.h"
#include "Door.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

USeraAnimation::USeraAnimation()
{
    speed = 0.0f;
    isInAir = false;
    isMoving = false;
    isInteractingWithDoor = false;
    
    // 오른손 IK 초기값 설정
    enableRightHandIK = false;
    rightHandIKAlpha = 0.0f;
    rightHandIKLocation = FVector::ZeroVector;
    
    // IK 설정 초기화
    ikTransitionSpeed = 5.0f;
    maxReachDistance = 100.0f;
}

void USeraAnimation::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // 캐릭터 참조 가져오기
    seraCharacter = Cast<ASera>(TryGetPawnOwner());

    if (seraCharacter == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("애니메이션 초기화 오류: 세라 캐릭터 참조가 없습니다"));

        return;
    }

    // 캐릭터 무브먼트 컴포넌트 참조 가져오기
    characterMovement = seraCharacter->GetCharacterMovement();

    if (characterMovement == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("애니메이션 초기화 오류: 캐릭터 무브먼트 컴포넌트 참조가 없습니다"));

        return;
    }
}

void USeraAnimation::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    // 필요한 참조가 유효할 때만 업데이트 실행
    if (seraCharacter && characterMovement)
    {
        UpdateMovementValues();
        UpdateDoorInteractionValues();
        UpdateRightHandIK(DeltaTime);
    }
}

void USeraAnimation::UpdateMovementValues()
{
    // 캐릭터 속도 계산
    FVector velocity = characterMovement->Velocity;
    speed = velocity.Size();
    
    // 이동 상태 확인 (3.0f 이상이면 이동 중)
    isMoving = speed > 3.0f;
    
    // 공중 상태 확인
    isInAir = characterMovement->IsFalling();
}

void USeraAnimation::OnDoorInteractionStarted(const FVector& handleLocation, ADoor* interactingDoor)
{
    // 문 상호작용 상태 활성화
    isInteractingWithDoor = true;
    enableRightHandIK = true;
    isTransitioningFromDoor = false;
    hasReachedTarget = false;

    // 상호작용 중인 문 참조 저장
    activeDoor = interactingDoor;

    // 손잡이 위치를 목표 위치로 설정
    targetRightHandIKLocation = handleLocation;

    // 현재 손 위치 저장
    if (seraCharacter)
    {
        USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();

        if (skeletalMesh != nullptr)
        {
            previousRightHandIKLocation = skeletalMesh->GetBoneLocation(FName("RightHandIndex1"), EBoneSpaces::WorldSpace);

            // 첫 프레임 위치 설정
            rightHandIKLocation = previousRightHandIKLocation;
        }
    }

    // 로컬 위치 계산
    rightHandLocalLocation = ConvertWorldToLocalBoneSpace(rightHandIKLocation, FName("RightHandIndex1"));

    // 디버그 메시지 출력
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
        FString::Printf(TEXT("문 상호작용 시작 - 손잡이 위치: %s"), *handleLocation.ToString()));

    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
        FString::Printf(TEXT("시작 손 위치: %s"), *previousRightHandIKLocation.ToString()));
}

void USeraAnimation::UpdateRightHandIK(float DeltaTime)
{
    // 캐릭터 참조 확인
    if (seraCharacter == nullptr)
        return;

    // 전환 타이머 업데이트
    if (isTransitioningFromDoor)
    {
        transitionTimer += DeltaTime;

        if (transitionTimer >= transitionDuration)
        {
            // 전환 완료 시 IK 비활성화
            isTransitioningFromDoor = false;
            enableRightHandIK = false;
            rightHandIKAlpha = 0.0f;
            activeDoor = nullptr;
        }
    }

    // IK 알파값 목표 설정
    float targetAlpha = 0.0f;

    if (isInteractingWithDoor)
        targetAlpha = 1.0f;

    // 전환 중 알파값 감소
    else if (isTransitioningFromDoor)
        targetAlpha = FMath::Lerp(1.0f, 0.0f, transitionTimer / transitionDuration);

    // 알파값 부드럽게 보간
    rightHandIKAlpha = FMath::FInterpTo(rightHandIKAlpha, targetAlpha, DeltaTime, ikTransitionSpeed);

    // IK 알파값이 임계값 이상이면 문 움직임 시작
    if (isInteractingWithDoor && !hasReachedTarget && rightHandIKAlpha >= handIKThreshold && activeDoor)
    {
        hasReachedTarget = true;

        // 문 움직임 시작 신호 전송
        activeDoor->StartDoorMovement();

        // 디버그 메시지 출력
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
            FString::Printf(TEXT("손 IK 임계값(%.2f) 도달. 문 움직임 시작합니다."), rightHandIKAlpha));
    }

    // IK 위치 업데이트
    if (isInteractingWithDoor || isTransitioningFromDoor)
    {
        USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();

        if (skeletalMesh == nullptr)
            return;

        if (isInteractingWithDoor)
        {
            // 초기 손 위치 설정
            if (targetRightHandIKLocation.IsZero())
            {
                previousRightHandIKLocation = skeletalMesh->GetBoneLocation(FName("RightHandIndex1"), EBoneSpaces::WorldSpace);
                rightHandIKLocation = previousRightHandIKLocation;
            }

            // 목표 위치 업데이트
            if (activeDoor)
                targetRightHandIKLocation = activeDoor->GetLeftPositionHandleWorldLocation();

            else
                targetRightHandIKLocation = GetDoorLeftPositionHandleLocation();

            // 손과 손잡이 거리 계산
            currentHandToHandleDistance = FVector::Dist(rightHandIKLocation, targetRightHandIKLocation);

            // 위치 부드럽게 보간
            rightHandIKLocation = FMath::VInterpTo(
                rightHandIKAlpha > 0.01f ? rightHandIKLocation : previousRightHandIKLocation,
                targetRightHandIKLocation,
                DeltaTime,
                ikTransitionSpeed);
        }

        else if (isTransitioningFromDoor)
        {
            // 전환 중 기본 손 위치로 복귀
            FVector defaultHandPos = skeletalMesh->GetBoneLocation(FName("RightHandIndex1"), EBoneSpaces::WorldSpace);

            // 전환 비율에 따른 위치 보간
            float transitionRatio = transitionTimer / transitionDuration;
            rightHandIKLocation = FMath::Lerp(lastValidHandPosition, defaultHandPos, transitionRatio);
        }

        // 월드 위치를 로컬 위치로 변환
        rightHandLocalLocation = ConvertWorldToLocalBoneSpace(rightHandIKLocation, FName("RightHandIndex1"));

        // 디버그 로그 출력 (주기적으로만 출력하여 화면 스팸 방지)
        if (FMath::Fmod(transitionTimer, 0.5f) < DeltaTime)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan,
                FString::Printf(TEXT("손 IK - 월드 위치: %s, 로컬 위치: %s, 알파값: %.2f, 전환 중: %s"),
                    *rightHandIKLocation.ToString(), *rightHandLocalLocation.ToString(), rightHandIKAlpha,
                    isTransitioningFromDoor ? TEXT("예") : TEXT("아니오")));
        }
    }

    else if (rightHandIKAlpha <= 0.01f)
    {
        // IK 비활성화 시 위치 초기화
        rightHandIKLocation = FVector::ZeroVector;
        rightHandLocalLocation = FVector::ZeroVector;
        hasReachedTarget = false;
    }
}

void USeraAnimation::UpdateDoorInteractionValues()
{
    // 캐릭터 참조 확인
    if (seraCharacter == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 상호작용 업데이트 오류: 세라 캐릭터 참조가 없습니다"));

        return;
    }

    // 가장 가까운 문 또는 활성 문 참조 가져오기
    ADoor* doorToCheck = activeDoor ? activeDoor : GetNearestDoor();

    if (doorToCheck)
    {
        // 문 상태 확인
        bool isDoorMoving = doorToCheck->IsMoving();
        bool isDoorPreparing = doorToCheck->IsPreparing();

        // 문 상호작용 시작 조건
        if ((isDoorMoving || isDoorPreparing) && !isInteractingWithDoor && !isTransitioningFromDoor)
        {
            isInteractingWithDoor = true;
            enableRightHandIK = true;
            hasReachedTarget = false;
            activeDoor = doorToCheck;
        }

        // 문 상호작용 종료 조건
        if (!isDoorMoving && !isDoorPreparing && isInteractingWithDoor)
        {
            isInteractingWithDoor = false;

            // 전환 시작을 위한 현재 손 위치 저장
            if (seraCharacter && !rightHandIKLocation.IsZero())
            {
                lastValidHandPosition = rightHandIKLocation;
                isTransitioningFromDoor = true;
                transitionTimer = 0.0f;

                // 디버그 메시지 출력
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
                    FString::Printf(TEXT("손 위치 전환 시작: %s에서 원래 위치로"), *lastValidHandPosition.ToString()));
            }
        }

        // 디버그 로그 출력 (주기적으로만 출력하여 화면 스팸 방지)
        if (isInteractingWithDoor && GEngine && FMath::Fmod(transitionTimer, 1.0f) < 0.01f)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
                FString::Printf(TEXT("문 상호작용 중 - 손잡이 위치: %s, 알파값: %.2f"),
                    *rightHandIKLocation.ToString(), rightHandIKAlpha));
        }
    }
    else if (!isTransitioningFromDoor)
    {
        // 문이 없으면 상호작용 해제
        isInteractingWithDoor = false;
        enableRightHandIK = false;
        activeDoor = nullptr;
    }
}

FVector USeraAnimation::ConvertWorldToLocalBoneSpace(const FVector& worldLocation, const FName& boneName)
{
    // 필요한 참조 확인
    if (seraCharacter == nullptr)
        return FVector::ZeroVector;

    USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();

    if (skeletalMesh == nullptr)
        return FVector::ZeroVector;

    // 본 트랜스폼 가져오기
    FTransform boneTransform = skeletalMesh->GetBoneTransform(skeletalMesh->GetBoneIndex(boneName));

    if (boneTransform.GetScale3D().IsNearlyZero())
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
            FString::Printf(TEXT("본 변환 실패: %s 본의 유효한 트랜스폼을 가져올 수 없습니다"), *boneName.ToString()));

        return FVector::ZeroVector;
    }

    // 월드 좌표를 본 로컬 좌표로 변환
    FVector localLocation = boneTransform.InverseTransformPosition(worldLocation);

    // 디버그 로그 출력 (주기적으로만 출력하여 화면 스팸 방지)
    if (FMath::Fmod(transitionTimer, 1.0f) < 0.01f)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan,
            FString::Printf(TEXT("좌표 변환 - 월드 위치: %s → 로컬 위치: %s (본: %s)"),
                *worldLocation.ToString(), *localLocation.ToString(), *boneName.ToString()));
    }

    return localLocation;
}

ADoor* USeraAnimation::GetNearestDoor()
{
    // 필요한 참조 확인
    if (seraCharacter == nullptr || seraCharacter->GetWorld() == nullptr) 
        return nullptr;

    ADoor* nearestDoor = nullptr;
    float nearestDistance = 500.0f;

    // 모든 문 액터 순회
    for (TActorIterator<ADoor> ActorItr(seraCharacter->GetWorld()); ActorItr; ++ActorItr)
    {
        ADoor* door = *ActorItr;

        if (door != nullptr)
        {
            float distance = FVector::Dist(seraCharacter->GetActorLocation(), door->GetActorLocation());

            if (distance < nearestDistance)
            {
                nearestDistance = distance;
                nearestDoor = door;
            }
        }
    }

    return nearestDoor;
}

FVector USeraAnimation::GetDoorLeftPositionHandleLocation()
{
    // 가장 가까운 문 찾기
    ADoor* nearestDoor = GetNearestDoor();

    // 문이 있으면 손잡이 위치 반환
    if (nearestDoor != nullptr)
        return nearestDoor->GetLeftPositionHandleWorldLocation();

    return FVector::ZeroVector;
}

bool USeraAnimation::IsTargetWithinReach(const FVector& targetLocation)
{
    // 필요한 참조와 위치 유효성 확인
    if (seraCharacter == nullptr || targetLocation.IsZero()) 
        return false;

    // 타겟까지 거리 계산
    float distanceToTarget = FVector::Dist(seraCharacter->GetActorLocation(), targetLocation);

    // 최대 도달 거리 내인지 확인
    return distanceToTarget <= maxReachDistance;
}

float USeraAnimation::GetHandToHandleDistance() const
{
    // 저장된 손과 손잡이 사이 거리 반환
    return currentHandToHandleDistance;
}