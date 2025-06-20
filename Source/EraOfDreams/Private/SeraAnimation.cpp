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
    
    // 오른손 IK 관련 초기값
    enableRightHandIK = false;
    rightHandIKAlpha = 0.0f;
    rightHandIKLocation = FVector::ZeroVector;
    
    // IK 설정
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
        UE_LOG(LogTemp, Warning, TEXT("seraCharacter is null in USeraAnimation::NativeInitializeAnimation"));
        return;
    }

    characterMovement = seraCharacter->GetCharacterMovement();
    if (characterMovement == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("characterMovement is null in USeraAnimation::NativeInitializeAnimation"));
        return;
    }
}

void USeraAnimation::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (seraCharacter && characterMovement)
    {
        UpdateMovementValues();
        UpdateDoorInteractionValues();
        UpdateRightHandIK(DeltaTime);
    }
}

void USeraAnimation::UpdateMovementValues()
{
    // 속도 계산
    FVector velocity = characterMovement->Velocity;
    speed = velocity.Size();
    
    // 이동 상태 확인
    isMoving = speed > 3.0f;
    
    // 공중에 있는지 확인
    isInAir = characterMovement->IsFalling();
}

void USeraAnimation::OnDoorInteractionStarted(const FVector& HandleLocation, ADoor* InteractingDoor)
{
    // 문 상호작용 상태 설정
    isInteractingWithDoor = true;
    enableRightHandIK = true;
    isTransitioningFromDoor = false; // 전환 상태 초기화
    hasReachedTarget = false; // 목표 도달 상태 초기화
    
    // 상호작용 중인 문 저장
    activeDoor = InteractingDoor;
    
    // IK 타겟 위치 설정 (목표 위치)
    targetRightHandIKLocation = HandleLocation;
    
    // 현재 손 위치 저장 (시작점)
    if (seraCharacter)
    {
        USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();
        if (skeletalMesh)
        {
            previousRightHandIKLocation = skeletalMesh->GetBoneLocation(FName("RightHandIndex1"), EBoneSpaces::WorldSpace);
            // 첫 프레임에서의 현재 위치 설정 (이후 보간됨)
            rightHandIKLocation = previousRightHandIKLocation;
        }
    }
    
    // 로컬 위치 업데이트
    rightHandLocalLocation = ConvertWorldToLocalBoneSpace(rightHandIKLocation, FName("RightHandIndex1"));
    
    // 디버그 메시지
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            FString::Printf(TEXT("Door interaction started - Handle location: %s"), *HandleLocation.ToString()));
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("Starting hand position: %s"), *previousRightHandIKLocation.ToString()));
    }
}

void USeraAnimation::UpdateRightHandIK(float DeltaTime)
{
    if (!seraCharacter) return;

    // 전환 타이머 업데이트
    if (isTransitioningFromDoor)
    {
        transitionTimer += DeltaTime;
        if (transitionTimer >= transitionDuration)
        {
            // 전환 완료
            isTransitioningFromDoor = false;
            enableRightHandIK = false;
            rightHandIKAlpha = 0.0f;
            activeDoor = nullptr; // 활성 문 참조 제거
        }
    }

    // IK 알파 값 부드럽게 전환
    float targetAlpha = 0.0f;
    
    if (isInteractingWithDoor)
    {
        targetAlpha = 1.0f;
    }
    else if (isTransitioningFromDoor)
    {
        // 전환 중일 때는 타이머에 따라 알파값 감소
        targetAlpha = FMath::Lerp(1.0f, 0.0f, transitionTimer / transitionDuration);
    }
    
    rightHandIKAlpha = FMath::FInterpTo(rightHandIKAlpha, targetAlpha, DeltaTime, ikTransitionSpeed);

    // IK 알파가 임계값에 도달하면 문 움직임 시작
    if (isInteractingWithDoor && !hasReachedTarget && rightHandIKAlpha >= handIKThreshold && activeDoor)
    {
        hasReachedTarget = true;
        
        // 문에 움직임 시작 신호 보내기
        activeDoor->StartDoorMovement();
        
        // 디버그 메시지
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
                FString::Printf(TEXT("Hand IK reached threshold (%.2f). Starting door movement."), rightHandIKAlpha));
        }
    }

    // 문과 상호작용 중이거나 전환 중일 때 IK 타겟 위치 업데이트
    if (isInteractingWithDoor || isTransitioningFromDoor)
    {
        USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();
        if (!skeletalMesh) return;
        
        if (isInteractingWithDoor)
        {
            // 현재 손 위치 저장 (시작점)
            if (targetRightHandIKLocation.IsZero())
            {
                previousRightHandIKLocation = skeletalMesh->GetBoneLocation(FName("RightHandIndex1"), EBoneSpaces::WorldSpace);
                rightHandIKLocation = previousRightHandIKLocation; // 첫 프레임에서의 현재 위치 설정
            }
            
            // 목표 위치 업데이트
            if (activeDoor)
                targetRightHandIKLocation = activeDoor->GetLeftPositionHandleWorldLocation();
            
            else
                targetRightHandIKLocation = GetDoorLeftPositionHandleLocation();
            
            // 현재 위치와 목표 위치 사이를 부드럽게 보간
            rightHandIKLocation = FMath::VInterpTo(
                rightHandIKAlpha > 0.01f ? rightHandIKLocation : previousRightHandIKLocation, 
                targetRightHandIKLocation, 
                DeltaTime, 
                ikTransitionSpeed);
        }

        else if (isTransitioningFromDoor)
        {
            // 전환 중에는 마지막 유효 위치에서 기본 손 위치로 전환
            FVector defaultHandPos = skeletalMesh->GetBoneLocation(FName("RightHandIndex1"), EBoneSpaces::WorldSpace);
            
            // transitionTimer 비율에 따라 부드럽게 보간
            float transitionRatio = transitionTimer / transitionDuration;
            rightHandIKLocation = FMath::Lerp(lastValidHandPosition, defaultHandPos, transitionRatio);
        }
        
        // 월드 위치를 오른손 본의 로컬 위치로 변환
        rightHandLocalLocation = ConvertWorldToLocalBoneSpace(rightHandIKLocation, FName("RightHandIndex1"));
        
        // 디버그 로그
        UE_LOG(LogTemp, Verbose, TEXT("Hand IK - World Pos: %s, Local Pos: %s, Alpha: %f, Transitioning: %s"), 
            *rightHandIKLocation.ToString(), *rightHandLocalLocation.ToString(), rightHandIKAlpha,
            isTransitioningFromDoor ? TEXT("true") : TEXT("false"));
    }

    else if (rightHandIKAlpha <= 0.01f)
    {
        // IK가 완전히 비활성화되면 위치 초기화
        rightHandIKLocation = FVector::ZeroVector;
        rightHandLocalLocation = FVector::ZeroVector;
        hasReachedTarget = false;
    }
}

void USeraAnimation::UpdateDoorInteractionValues()
{
    if (seraCharacter)
    {
        // 가장 가까운 문 찾기 또는 활성 문 사용
        ADoor* doorToCheck = activeDoor ? activeDoor : GetNearestDoor();
        
        if (doorToCheck)
        {
            // 문이 움직이고 있는지 확인
            bool isDoorMoving = doorToCheck->IsMoving();
            bool isDoorPreparing = doorToCheck->IsPreparing();
            
            // 문이 움직이고 있거나 준비 중이면 상호작용 상태 유지
            if ((isDoorMoving || isDoorPreparing) && !isInteractingWithDoor && !isTransitioningFromDoor)
            {
                isInteractingWithDoor = true;
                enableRightHandIK = true;
                hasReachedTarget = false;
                activeDoor = doorToCheck;
            }
            
            // 문 이동이 끝나면 전환 상태로 설정
            if (!isDoorMoving && !isDoorPreparing && isInteractingWithDoor)
            {
                isInteractingWithDoor = false;
                
                // 현재 손 위치를 마지막 유효 위치로 저장
                if (seraCharacter && !rightHandIKLocation.IsZero())
                {
                    lastValidHandPosition = rightHandIKLocation;
                    isTransitioningFromDoor = true;
                    transitionTimer = 0.0f;
                    
                    // 디버그 메시지
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, 
                            FString::Printf(TEXT("Starting hand transition from: %s"), *lastValidHandPosition.ToString()));
                    }
                }
            }
            
            // 디버그 정보 출력 (선택사항)
            if (isInteractingWithDoor)
            {
                UE_LOG(LogTemp, Log, TEXT("SeraAnimation: Interacting with door - Handle Location: %s, Alpha: %.2f"), 
                    *rightHandIKLocation.ToString(), rightHandIKAlpha);
            }
        }

        else if (!isTransitioningFromDoor)
        {
            isInteractingWithDoor = false;
            enableRightHandIK = false;
            activeDoor = nullptr;
        }
    }
}

FVector USeraAnimation::ConvertWorldToLocalBoneSpace(const FVector& WorldLocation, const FName& BoneName)
{
    if (!seraCharacter) return FVector::ZeroVector;
    
    USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();

    if (!skeletalMesh) 
        return FVector::ZeroVector;
    
    // 해당 본의 트랜스폼 가져오기
    FTransform boneTransform = skeletalMesh->GetBoneTransform(skeletalMesh->GetBoneIndex(BoneName));

    if (boneTransform.GetScale3D().IsNearlyZero()) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get valid bone transform for %s"), *BoneName.ToString());

        return FVector::ZeroVector;
    }
    
    // 월드 좌표를 본의 로컬 좌표로 변환
    FVector localLocation = boneTransform.InverseTransformPosition(WorldLocation);
    
    // 디버그 정보
    UE_LOG(LogTemp, Verbose, TEXT("Converting World Pos: %s to Local Pos: %s for bone: %s"),
        *WorldLocation.ToString(), *localLocation.ToString(), *BoneName.ToString());
        
    return localLocation;
}

ADoor* USeraAnimation::GetNearestDoor()
{
    if (!seraCharacter || !seraCharacter->GetWorld()) return nullptr;

    ADoor* nearestDoor = nullptr;
    float nearestDistance = 500.0f;

    for (TActorIterator<ADoor> ActorItr(seraCharacter->GetWorld()); ActorItr; ++ActorItr)
    {
        ADoor* door = *ActorItr;
        if (door)
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
    ADoor* nearestDoor = GetNearestDoor();

    if (nearestDoor)
        return nearestDoor->GetLeftPositionHandleWorldLocation();

    return FVector::ZeroVector;
}

bool USeraAnimation::IsTargetWithinReach(const FVector& TargetLocation)
{
    if (!seraCharacter || TargetLocation.IsZero()) return false;

    float distanceToTarget = FVector::Dist(seraCharacter->GetActorLocation(), TargetLocation);

    return distanceToTarget <= maxReachDistance;
}