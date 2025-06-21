#include "Door.h"
#include "KeyInputManager.h"
#include "Sera.h"
#include "SeraAnimation.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 문 메시 컴포넌트 생성 및 루트로 설정
    doorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = doorMesh;

    // 플레이어 감지용 상호작용 박스 생성
    interactionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    interactionBox->SetupAttachment(RootComponent);
    
    // 초기 박스 크기와 위치 설정
    interactionBox->SetBoxExtent(interactionExtent);
    interactionBox->SetRelativeLocation(interactionOffset);

    // 박스 컴포넌트 충돌 설정
    interactionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    interactionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    interactionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    interactionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    interactionBox->SetGenerateOverlapEvents(true);
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();
    
    // 문의 초기, 현재, 목표 회전값 설정
    initialRotation = GetActorRotation();
    currentRotation = initialRotation;
    targetRotation = initialRotation;

    // 플레이어 캐릭터 참조 가져오기
    playerCharacter = Cast<ASera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    
    if (playerCharacter == nullptr)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Door: 플레이어 캐릭터를 찾을 수 없습니다"));

    // 메시 크기에 맞게 상호작용 박스 조정
    if (doorMesh && doorMesh->GetStaticMesh())
    {
        // 메시 경계 상자 가져오기
        FBoxSphereBounds meshBounds = doorMesh->GetStaticMesh()->GetBounds();
        FVector meshExtent = meshBounds.BoxExtent;
        
        // 상호작용 영역을 메시보다 20% 크게 설정
        FVector adjustedExtent = meshExtent * 1.2f;
        
        // 높이는 문 높이의 절반으로 설정
        adjustedExtent.Z = meshExtent.Z * 0.5f;
        
        // 박스 크기 설정
        interactionBox->SetBoxExtent(adjustedExtent);
        
        // 박스 위치를 문 중앙에 맞춤
        FVector adjustedOffset = interactionOffset;
        adjustedOffset.Z = 0;
        
        // 문 앞쪽으로 박스 위치 조정
        FVector forwardOffset = GetActorForwardVector() * adjustedExtent.X * 0.5f;
        adjustedOffset += forwardOffset;
        
        interactionBox->SetRelativeLocation(adjustedOffset);
        
        // 콜라이더 크기 조정 디버그 메시지
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("Door 콜라이더 크기 조정: %s, 위치: %s"), 
            *adjustedExtent.ToString(), *adjustedOffset.ToString()));
    }

    // 오버랩 이벤트 등록
    interactionBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnInteractionBeginOverlap);
    interactionBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnInteractionEndOverlap);

    // 상호작용 키 등록
    KeyInputManager::AddEvent(EKeys::G, this, &ADoor::ToggleDoor);
    
    // 상호작용 영역 시각화
    DrawDebugBox(
        GetWorld(),
        interactionBox->GetComponentLocation(),
        interactionBox->GetScaledBoxExtent(),
        FQuat(interactionBox->GetComponentRotation()),
        FColor::Green,
        true,
        -1,
        0,
        2.0f
    );
    
    // 초기화 완료 메시지
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
        FString::Printf(TEXT("Door 초기화 완료 - 위치: %s"), *GetActorLocation().ToString()));
}

void ADoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{   
    Super::EndPlay(EndPlayReason);
}

void ADoor::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // 문 회전 처리
    if (isMoving)
    {
        // 이전 회전값 저장
        FRotator prevRotation = currentRotation;

        // 부드러운 회전 보간 적용
        currentRotation = FMath::RInterpTo(currentRotation, targetRotation, deltaTime, doorSpeed);
        SetActorRotation(currentRotation);

        // 문 상호작용 중 플레이어 이동 처리
        if (playerCharacter && playerCharacter->IsInteractingWithDoor())
        {
            // 문이 닫히는 중인지 확인
            bool isClosing = !isOpen;

            if (isClosing)
            {
                // 문 닫힐 때는 플레이어 이동 멈춤
                playerCharacter->StartDoorInteractionMovement(0.0f, 0.0f);

                if (FMath::Fmod(debugTimeSinceLastCheck, 0.5f) < deltaTime)
                    GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("문 닫는 중: 플레이어 이동 없음"));
            }

            // 문 여는 중일 때 플레이어 이동 관리
            else 
            {
                // 손과 손잡이 거리 확인
                bool shouldMove = true;
                float handToHandleDistance = 0.0f;

                USkeletalMeshComponent* skeletalMesh = playerCharacter->GetMesh();

                if (skeletalMesh != nullptr)
                {
                    USeraAnimation* seraAnimInstance = Cast<USeraAnimation>(skeletalMesh->GetAnimInstance());

                    if (seraAnimInstance != nullptr)
                    {
                        // 애니메이션에서 손-손잡이 거리 가져오기
                        handToHandleDistance = seraAnimInstance->GetHandToHandleDistance();

                        // 거리가 임계값 이하면 이동 중지
                        float distanceThreshold = 10.0f;
                        shouldMove = handToHandleDistance > distanceThreshold;

                        // 손-손잡이 거리 디버그 메시지
                        if (FMath::Fmod(debugTimeSinceLastCheck, 0.5f) < deltaTime)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan,
                                FString::Printf(TEXT("문 여는 중: 손-손잡이 거리: %.2f, 이동 %s"),
                                    handToHandleDistance, shouldMove ? TEXT("필요") : TEXT("불필요")));
                        }
                    }
                }

                // 거리에 따라 이동 처리
                if (shouldMove)
                {
                    // 문 회전 진행도 계산 (0-1 사이 값)
                    float rotationProgress = FMath::Abs((currentRotation.Yaw - initialRotation.Yaw) / openAngle);

                    // 회전 방향에 따른 좌우 이동 계산
                    float sideMovement = 0.0f;
                    
                    // 문과 플레이어의 상대적 위치 계산
                    FVector doorToPlayer = playerCharacter->GetActorLocation() - GetActorLocation();
                    FVector doorRight = GetActorRightVector();
                    
                    // 플레이어가 문의 어느 쪽에 있는지 내적으로 판단
                    float rightDot = FVector::DotProduct(doorToPlayer.GetSafeNormal(), doorRight);
                    
                    // 문이 열리는 방향과 플레이어의 위치에 따라 좌우 이동 방향 결정
                    bool openingToRight = openAngle > 0;
                    bool playerOnRight = rightDot > 0;
                    
                    // 문이 열리는 방향과 플레이어 위치에 따른 좌우 이동 결정
                    if ((openingToRight && playerOnRight) || (!openingToRight && !playerOnRight)) {
                        // 문이 플레이어 쪽으로 열리면 약간 반대 방향으로 이동
                        sideMovement = -0.3f * rotationProgress;
                    } else {
                        // 문이 플레이어 반대쪽으로 열리면 문 쪽으로 이동
                        sideMovement = 0.2f * rotationProgress;
                    }
                    
                    // 전진 속도 계산
                    float moveSpeed = 0.0f;

                    // 문 열기 초반에 속도 증가
                    if (rotationProgress < 0.3f)
                        moveSpeed = FMath::Lerp(0.15f, 0.6f, rotationProgress / 0.3f);
                    // 문 열기 후반에 속도 감소
                    else if (rotationProgress > 0.7f)
                        moveSpeed = FMath::Lerp(0.6f, 0.15f, (rotationProgress - 0.7f) / 0.3f);
                    // 문 열기 중간에 일정 속도 유지
                    else
                        moveSpeed = 0.6f;

                    // 속도 범위 제한
                    moveSpeed = FMath::Clamp(moveSpeed, 0.15f, 0.6f);
                    sideMovement = FMath::Clamp(sideMovement, -0.4f, 0.4f);

                    // 플레이어 이동 속도 적용 (앞뒤 및 좌우 이동)
                    playerCharacter->StartDoorInteractionMovement(moveSpeed, sideMovement);
                    
                    // 좌우 이동 디버그 정보
                    if (FMath::Fmod(debugTimeSinceLastCheck, 0.5f) < deltaTime)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green,
                            FString::Printf(TEXT("문 통과: 전진 %.2f, 좌우 %.2f, 진행도 %.2f%%"),
                            moveSpeed, sideMovement, rotationProgress * 100.0f));
                    }
                }
                else
                {
                    playerCharacter->StartDoorInteractionMovement(0.0f, 0.0f);
                }
            }
        }

        // 목표 회전에 거의 도달했는지 확인
        if (FMath::IsNearlyEqual(currentRotation.Yaw, targetRotation.Yaw, 1.0f))
        {
            // 문 회전 완료 처리
            currentRotation = targetRotation;
            SetActorRotation(currentRotation);
            isMoving = false;

            // 플레이어 이동 중지
            if (playerCharacter && playerCharacter->IsInteractingWithDoor())
            {
                playerCharacter->StopDoorInteractionMovement();

                // 문 열기/닫기 완료 메시지
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                    FString::Printf(TEXT("문 %s 완료"), isOpen ? TEXT("열기") : TEXT("닫기")));
            }
        }
    }
    
    // 디버그 타이머 갱신
    debugTimeSinceLastCheck += deltaTime;

    // 주기적 디버그 정보 출력
    if (debugTimeSinceLastCheck >= 1.0f)
    {
        debugTimeSinceLastCheck = 0.0f;
        
        if (playerCharacter)
        {
            // 플레이어와 문 사이 거리 계산
            float distToPlayer = FVector::Dist(GetActorLocation(), playerCharacter->GetActorLocation());

            if (distToPlayer < 500.0f)
            {
                // 플레이어 근접 상태 메시지
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, 
                    FString::Printf(TEXT("문: 플레이어가 가까이 있음 - 거리: %.2f, 상호작용 가능: %s"), 
                    distToPlayer, isPlayerInRange ? TEXT("예") : TEXT("아니오")));
                
                // 가까이 있지만 상호작용 불가능한 경우 수동 충돌 검사
                if (!isPlayerInRange && distToPlayer < 200.0f)
                {
                    // 수동 AABB 충돌 검사
                    FVector playerLocation = playerCharacter->GetActorLocation();
                    FVector boxLocation = interactionBox->GetComponentLocation();
                    FVector boxExtent = interactionBox->GetScaledBoxExtent();
                    
                    // 각 축별 충돌 검사
                    bool overlapX = FMath::Abs(playerLocation.X - boxLocation.X) < (boxExtent.X + 50.0f);
                    bool overlapY = FMath::Abs(playerLocation.Y - boxLocation.Y) < (boxExtent.Y + 50.0f);
                    bool overlapZ = FMath::Abs(playerLocation.Z - boxLocation.Z) < (boxExtent.Z + 100.0f);
                    
                    // 모든 축에서 충돌하면 수동으로 상호작용 활성화
                    if (overlapX && overlapY && overlapZ)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문: 수동 충돌 감지 - 플레이어와 충돌 중이지만 이벤트가 발생하지 않음"));
                        
                        // 축별 충돌 상태 출력
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
                            FString::Printf(TEXT("충돌 상세: X축[%s] Y축[%s] Z축[%s]"), 
                            overlapX ? TEXT("충돌") : TEXT("미충돌"),
                            overlapY ? TEXT("충돌") : TEXT("미충돌"),
                            overlapZ ? TEXT("충돌") : TEXT("미충돌")));
                        
                        // 위치 차이 출력
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, 
                            FString::Printf(TEXT("위치 차이: X[%.1f] Y[%.1f] Z[%.1f]"), 
                            FMath::Abs(playerLocation.X - boxLocation.X),
                            FMath::Abs(playerLocation.Y - boxLocation.Y),
                            FMath::Abs(playerLocation.Z - boxLocation.Z)));
                        
                        // 상호작용 가능 상태로 설정
                        isPlayerInRange = true;
                    }
                }
            }
        }
    }
}

void ADoor::OnInteractionBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
    UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& sweepResult)
{
    if (otherActor == nullptr) 
        return;
    
    // 오버랩 감지 메시지
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Door: 오버랩 감지 - Actor: %s"), *otherActor->GetName()));

    // 일반 플레이어 캐릭터 체크
    ACharacter* character = Cast<ACharacter>(otherActor);

    if (character && character->IsPlayerControlled())
    {
        isPlayerInRange = true;

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("G키를 눌러 문과 상호작용하세요"));
    }
    
    // Sera 캐릭터 체크
    ASera* seraPlayer = Cast<ASera>(otherActor);

    if (seraPlayer != nullptr)
    {
        isPlayerInRange = true;

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("세라 발견: G키를 눌러 문과 상호작용하세요"));
    }
}

void ADoor::OnInteractionEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
    UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
    if (otherActor == nullptr) 
        return;
    
    // 오버랩 종료 메시지
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, FString::Printf(TEXT("Door: 오버랩 종료 - Actor: %s"), *otherActor->GetName()));
    
    // 플레이어 캐릭터 체크
    ACharacter* character = Cast<ACharacter>(otherActor);

    if (character && character->IsPlayerControlled())
        isPlayerInRange = false;
    
    // Sera 캐릭터 체크
    ASera* seraPlayer = Cast<ASera>(otherActor);

    if (seraPlayer != nullptr)
        isPlayerInRange = false;
}

void ADoor::ToggleDoor()
{
    // 유효한 조건에서만 문 상호작용 실행
    if (isPlayerInRange && !isMoving && !isPreparing && playerCharacter)
    {
        isPreparing = true;
        shouldOpen = !isOpen;

        // 문 손잡이 위치 가져오기
        FVector leftHandleLocation = GetLeftPositionHandleWorldLocation();
        
        // 애니메이션 시스템에 문 상호작용 시작 알림
        if (playerCharacter != nullptr)
        {
            USkeletalMeshComponent* seraSkeletalMesh = playerCharacter->GetMesh();

            if (seraSkeletalMesh != nullptr)
            {
                USeraAnimation* seraAnimInstance = Cast<USeraAnimation>(seraSkeletalMesh->GetAnimInstance());

                if (seraAnimInstance != nullptr)
                    seraAnimInstance->OnDoorInteractionStarted(leftHandleLocation, this);
            }
        }

        // 문 상태 디버그 메시지
        FString stateMessage = shouldOpen ? TEXT("문 열기 준비 중") : TEXT("문 닫기 준비 중");
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, stateMessage);
        
        // 손잡이 위치 디버그 메시지
        FString locationMessage = FString::Printf(TEXT("왼쪽 손잡이 위치: %s"), *leftHandleLocation.ToString());
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, locationMessage);
    }

    else
    {
        // 상호작용 실패 원인 출력
        if (!isPlayerInRange)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 토글 실패 : 플레이어가 범위 내에 없습니다"));

        else if (isMoving)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 토글 실패 : 문이 이미 움직이고 있습니다"));

        else if (isPreparing)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 토글 실패 : 문이 이미 준비 중입니다"));

        else if (!playerCharacter)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 토글 실패 : 플레이어 캐릭터 참조가 없습니다"));
    }
}

void ADoor::StartDoorMovement()
{
    // 준비 상태가 아니면 실행 중지
    if (!isPreparing) 
        return;
    
    // 문 상태 변경
    isMoving = true;
    isPreparing = false;
    isOpen = shouldOpen;

    // 플레이어 이동 시작
    if (playerCharacter != nullptr)
    {
        // 문 열기/닫기에 따라 다른 이동 속도 설정
        float initialSpeed = isOpen ? 0.8f : 0.6f;
        float initialSideSpeed = 0.0f;
        
        // 문과 플레이어의 상대적 위치 계산하여 초기 좌우 이동 설정
        FVector doorToPlayer = playerCharacter->GetActorLocation() - GetActorLocation();
        FVector doorRight = GetActorRightVector();
        float rightDot = FVector::DotProduct(doorToPlayer.GetSafeNormal(), doorRight);
        
        // 문이 열리는 방향에 따라 초기 좌우 이동 조정
        bool openingToRight = openAngle > 0;
        bool playerOnRight = rightDot > 0;
        
        if ((openingToRight && playerOnRight) || (!openingToRight && !playerOnRight)) {
            initialSideSpeed = -0.1f;  // 문이 플레이어 쪽으로 열리면 약간 반대로 이동
        }
        
        playerCharacter->StartDoorInteractionMovement(initialSpeed, initialSideSpeed);
        
        // 플레이어 이동 시작 메시지
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
            FString::Printf(TEXT("문 움직임 시작 - 플레이어 이동 시작 (전진: %.2f, 좌우: %.2f)"), 
            initialSpeed, initialSideSpeed));
    }

    // 목표 회전값 설정
    if (isOpen)
        targetRotation = initialRotation + FRotator(0.0f, openAngle, 0.0f);

    else
        targetRotation = initialRotation;
    
    // 문 상태 변경 메시지
    FString stateMessage = isOpen ? TEXT("문 열기 시작") : TEXT("문 닫기 시작");
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, stateMessage);
}

FVector ADoor::GetLeftPositionHandleWorldLocation() const
{
    // 메시 컴포넌트 유효성 확인
    if (doorMesh == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Door 메시가 null입니다."));

        return GetActorLocation();
    }

    // 손잡이 소켓 위치 가져오기
    FName leftHandleSocketName = TEXT("LeftPositionHandle");
    if (doorMesh->DoesSocketExist(leftHandleSocketName))
    {
        FTransform socketTransform = doorMesh->GetSocketTransform(leftHandleSocketName);

        return socketTransform.GetLocation();
    }

    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("LeftPositionHandle socket이 문 메시에서 찾을 수 없습니다."));

        return GetActorLocation();
    }
}

void ADoor::TryToggleDoor()
{
    // 플레이어 거리 확인 후 문 상호작용 시도
    if (playerCharacter != nullptr)
    {
        float distToPlayer = FVector::Dist(GetActorLocation(), playerCharacter->GetActorLocation());
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Door: 플레이어까지의 거리: %.2f"), distToPlayer));

        // 플레이어가 가까우면 강제 상호작용
        if (distToPlayer < 300.0f)
        {
            isPlayerInRange = true;
            ToggleDoor();
        }

        else
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 토글 실패 : 플레이어가 너무 멀리 있습니다"));
    }

    else
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("문 토글 실패 : 플레이어 캐릭터 참조가 없습니다"));
}