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

void USeraAnimation::OnDoorInteractionStarted(const FVector& LeftHandleLocation)
{
    // 문 상호작용 상태 설정
    isInteractingWithDoor = true;
    enableRightHandIK = true;
    
    // IK 타겟 위치 직접 설정 (월드 좌표)
    rightHandIKLocation = LeftHandleLocation;
    
    // 월드 위치를 오른손 본의 로컬 위치로 변환
    rightHandLocalLocation = ConvertWorldToLocalBoneSpace(LeftHandleLocation, FName("RightHandIndex1"));
    
    // 디버그 메시지
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            FString::Printf(TEXT("Door interaction started - Handle location: %s"), *LeftHandleLocation.ToString()));
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("RightHandIndex1 local location: %s"), *rightHandLocalLocation.ToString()));
    }
}

void USeraAnimation::UpdateDoorInteractionValues()
{
    if (seraCharacter)
    {
        // 가장 가까운 문 찾기
        ADoor* nearestDoor = GetNearestDoor();
        
        if (nearestDoor)
        {
            // 문이 움직이고 있는지 확인 - 움직이지 않으면 상호작용 종료
            bool isDoorMoving = nearestDoor->IsMoving();
            
            if (!isDoorMoving && isInteractingWithDoor)
            {
                // 문 이동이 끝나면 상호작용 상태 종료
                isInteractingWithDoor = false;
                enableRightHandIK = false;
            }
            
            // 디버그 정보 출력 (선택사항)
            if (isInteractingWithDoor)
            {
                UE_LOG(LogTemp, Log, TEXT("SeraAnimation: Interacting with door - Handle Location: %s"), *rightHandIKLocation.ToString());
            }
        }
        else
        {
            isInteractingWithDoor = false;
            enableRightHandIK = false;
        }
    }
}

void USeraAnimation::UpdateRightHandIK(float DeltaTime)
{
    if (!seraCharacter) return;

    // IK 알파 값 부드럽게 전환
    float targetAlpha = isInteractingWithDoor ? 1.0f : 0.0f;
    rightHandIKAlpha = FMath::FInterpTo(rightHandIKAlpha, targetAlpha, DeltaTime, ikTransitionSpeed);

    // 문과 상호작용 중일 때 IK 타겟 위치 업데이트
    if (isInteractingWithDoor)
    {
        // 월드 위치 업데이트
        rightHandIKLocation = GetDoorLeftPositionHandleLocation();
        
        // 월드 위치를 오른손 본의 로컬 위치로 변환
        rightHandLocalLocation = ConvertWorldToLocalBoneSpace(rightHandIKLocation, FName("RightHandIndex1"));
        
        // 디버그 로그
        UE_LOG(LogTemp, Verbose, TEXT("Hand IK - World Pos: %s, Local Pos: %s"), 
            *rightHandIKLocation.ToString(), *rightHandLocalLocation.ToString());
    }
    else
    {
        rightHandIKLocation = FVector::ZeroVector;
        rightHandLocalLocation = FVector::ZeroVector;
    }
}

// 월드 위치를 특정 본의 로컬 좌표계로 변환하는 함수 구현
FVector USeraAnimation::ConvertWorldToLocalBoneSpace(const FVector& WorldLocation, const FName& BoneName)
{
    if (!seraCharacter) return FVector::ZeroVector;
    
    USkeletalMeshComponent* skeletalMesh = seraCharacter->GetMesh();
    if (!skeletalMesh) return FVector::ZeroVector;
    
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
    {
        return nearestDoor->GetLeftPositionHandleWorldLocation();
    }
    return FVector::ZeroVector;
}

bool USeraAnimation::IsTargetWithinReach(const FVector& TargetLocation)
{
    if (!seraCharacter || TargetLocation.IsZero()) return false;

    float distanceToTarget = FVector::Dist(seraCharacter->GetActorLocation(), TargetLocation);
    return distanceToTarget <= maxReachDistance;
}

FVector USeraAnimation::GetRightHandIKLocation() const
{
    if (!seraCharacter)
        return FVector::ZeroVector;
    
    // 월드 좌표를 캐릭터의 로컬 좌표계로 정확하게 변환
    // InverseTransformPosition은 위치, 회전, 스케일을 모두 고려하여 변환
    FVector localLocation = rightHandIKLocation - seraCharacter->GetActorTransform().GetLocation();

    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,
		FString::Printf(TEXT("Right Hand IK Location: %s"), *localLocation.ToString()));
    
    return rightHandIKLocation;
}