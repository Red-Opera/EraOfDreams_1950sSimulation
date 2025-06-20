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

    // 메시 컴포넌트 생성
    doorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = doorMesh;

    // 상호작용 박스 컴포넌트 생성
    interactionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    interactionBox->SetupAttachment(RootComponent);
    
    // 초기 설정은 생성자에서 하고, 메시가 로드된 후에 실제 크기로 조정
    interactionBox->SetBoxExtent(interactionExtent);
    interactionBox->SetRelativeLocation(interactionOffset);

    // 충돌 설정
    interactionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    interactionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    interactionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    interactionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    interactionBox->SetGenerateOverlapEvents(true);
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();
    
    // 초기 회전값 저장
    initialRotation = GetActorRotation();
    currentRotation = initialRotation;
    targetRotation = initialRotation;

    // 플레이어 참조 가져오기
    playerCharacter = Cast<ASera>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    
    if (!playerCharacter)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Door: 플레이어 캐릭터를 찾을 수 없습니다"));
    }

    // 메시 콜라이더 크기에 맞게 상호작용 박스 크기 조정
    if (doorMesh && doorMesh->GetStaticMesh())
    {
        // 메시의 경계 상자 가져오기
        FBoxSphereBounds meshBounds = doorMesh->GetStaticMesh()->GetBounds();
        FVector meshExtent = meshBounds.BoxExtent;
        
        // 경계 상자에 약간의 여유 공간 추가 (플레이어가 더 쉽게 상호작용할 수 있도록)
        FVector adjustedExtent = meshExtent * 1.2f;
        
        // 높이(Z) 조정 - 문 높이의 절반만 사용하고 위치를 조정
        adjustedExtent.Z = meshExtent.Z * 0.5f;
        
        // 상호작용 박스 크기 및 위치 설정
        interactionBox->SetBoxExtent(adjustedExtent);
        
        // 위치 조정 - 문의 중앙에서 약간 앞으로 이동
        FVector adjustedOffset = interactionOffset;
        adjustedOffset.Z = 0; // 높이는 문의 중앙에 맞춤
        
        // 문 방향에 따라 앞쪽으로 약간 오프셋 조정
        FVector forwardOffset = GetActorForwardVector() * adjustedExtent.X * 0.5f;
        adjustedOffset += forwardOffset;
        
        interactionBox->SetRelativeLocation(adjustedOffset);
        
        // 디버그 메시지
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("Door 콜라이더 크기 조정: %s, 위치: %s"), 
            *adjustedExtent.ToString(), *adjustedOffset.ToString()));
    }

    // 오버랩 이벤트 바인딩
    interactionBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnInteractionBeginOverlap);
    interactionBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnInteractionEndOverlap);

    // G키 이벤트 등록
    KeyInputManager::AddEvent(EKeys::G, this, &ADoor::ToggleDoor);
    
    // 디버그용 박스 표시
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
    
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
        FString::Printf(TEXT("Door 초기화 완료 - 위치: %s"), *GetActorLocation().ToString()));
}

void ADoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{   
    Super::EndPlay(EndPlayReason);
}

void ADoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 문이 움직이고 있을 때 회전 보간
    if (isMoving)
    {
        currentRotation = FMath::RInterpTo(currentRotation, targetRotation, DeltaTime, doorSpeed);
        SetActorRotation(currentRotation);

        // 목표 회전에 거의 도달했는지 확인
        if (FMath::IsNearlyEqual(currentRotation.Yaw, targetRotation.Yaw, 1.0f))
        {
            SetActorRotation(targetRotation);
            isMoving = false;
        }
    }
    
    // 디버깅: 주기적으로 플레이어 위치 확인
    debugTimeSinceLastCheck += DeltaTime;
    if (debugTimeSinceLastCheck >= 1.0f)
    {
        debugTimeSinceLastCheck = 0.0f;
        
        if (playerCharacter)
        {
            float distToPlayer = FVector::Dist(GetActorLocation(), playerCharacter->GetActorLocation());
            if (distToPlayer < 500.0f)
            {
                UE_LOG(LogTemp, Warning, TEXT("Door: 플레이어가 문과 가까이 있음 - 거리: %.2f, 상호작용 가능: %s"), 
                    distToPlayer, isPlayerInRange ? TEXT("예") : TEXT("아니오"));
                
                // 플레이어가 가까이 있지만 isPlayerInRange가 false인 경우 수동으로 오버랩 검사
                if (!isPlayerInRange && distToPlayer < 200.0f)
                {
                    // 플레이어 캡슐과 상호작용 박스 사이의 오버랩 수동 테스트
                    FVector playerLocation = playerCharacter->GetActorLocation();
                    FVector boxLocation = interactionBox->GetComponentLocation();
                    FVector boxExtent = interactionBox->GetScaledBoxExtent();
                    
                    // 간단한 AABB 충돌 검사
                    bool overlapX = FMath::Abs(playerLocation.X - boxLocation.X) < (boxExtent.X + 50.0f);
                    bool overlapY = FMath::Abs(playerLocation.Y - boxLocation.Y) < (boxExtent.Y + 50.0f);
                    bool overlapZ = FMath::Abs(playerLocation.Z - boxLocation.Z) < (boxExtent.Z + 100.0f);
                    
                    if (overlapX && overlapY && overlapZ)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Door: 수동 충돌 감지 - 플레이어와 충돌 중이지만 이벤트가 발생하지 않음"));
                        
                        // 수동으로 플레이어 범위 내 설정
                        isPlayerInRange = true;
                        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("수동 감지: Press G to interact with door"));
                    }
                }
            }
        }
    }
}

void ADoor::OnInteractionBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
    UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& sweepResult)
{
    if (!otherActor) return;
    
    // 디버그 메시지 (모든 오버랩)
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, 
        FString::Printf(TEXT("Door: 오버랩 감지 - Actor: %s"), *otherActor->GetName()));

    // 플레이어 캐릭터 체크
    ACharacter* character = Cast<ACharacter>(otherActor);
    if (character && character->IsPlayerControlled())
    {
        isPlayerInRange = true;
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Press G to interact with door"));
    }
    
    // 또는 구체적인 Sera 클래스 확인
    ASera* seraPlayer = Cast<ASera>(otherActor);
    if (seraPlayer)
    {
        isPlayerInRange = true;
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Sera found: Press G to interact with door"));
    }
}

void ADoor::OnInteractionEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
    UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
    if (!otherActor) return;
    
    // 디버그 메시지
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, 
        FString::Printf(TEXT("Door: 오버랩 종료 - Actor: %s"), *otherActor->GetName()));
    
    // 플레이어 캐릭터 체크
    ACharacter* character = Cast<ACharacter>(otherActor);
    if (character && character->IsPlayerControlled())
    {
        isPlayerInRange = false;
    }
    
    // 또는 구체적인 Sera 클래스 확인
    ASera* seraPlayer = Cast<ASera>(otherActor);
    if (seraPlayer)
    {
        isPlayerInRange = false;
    }
}

void ADoor::ToggleDoor()
{
    // 플레이어가 범위 내에 있고 문이 움직이거나 준비 중이지 않을 때만 토글
    if (isPlayerInRange && !isMoving && !isPreparing && playerCharacter)
    {
        isPreparing = true;
        shouldOpen = !isOpen; // 문을 열 것인지 닫을 것인지 저장

        // LeftPositionHandle 소켓 위치 가져오기
        FVector leftHandleLocation = GetLeftPositionHandleWorldLocation();
        
        // SeraAnimation에 문 상호작용 시작 알림
        if (playerCharacter)
        {
            USkeletalMeshComponent* seraSkeletalMesh = playerCharacter->GetMesh();
            if (seraSkeletalMesh)
            {
                USeraAnimation* seraAnimInstance = Cast<USeraAnimation>(seraSkeletalMesh->GetAnimInstance());
                if (seraAnimInstance)
                {
                    seraAnimInstance->OnDoorInteractionStarted(leftHandleLocation, this);
                }
            }
        }

        // 디버그 메시지 
        FString stateMessage = shouldOpen ? TEXT("Preparing to Open Door") : TEXT("Preparing to Close Door");
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, stateMessage);
        
        FString locationMessage = FString::Printf(TEXT("LeftPositionHandle Location: %s"), *leftHandleLocation.ToString());
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, locationMessage);
    }
    else
    {
        // 왜 토글이 안되는지 디버그 메시지
        if (!isPlayerInRange)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Door toggle failed: Player not in range"));
        else if (isMoving)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Door toggle failed: Door is already moving"));
        else if (isPreparing)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Door toggle failed: Door is being prepared"));
        else if (!playerCharacter)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Door toggle failed: Player reference is invalid"));
    }
}

// 실제 문 움직임을 시작하는 함수 (손 IK가 완료된 후 호출됨)
void ADoor::StartDoorMovement()
{
    if (!isPreparing) return;
    
    isMoving = true;
    isPreparing = false;
    isOpen = shouldOpen;

    // 문 열기/닫기
    if (isOpen)
        targetRotation = initialRotation + FRotator(0.0f, openAngle, 0.0f);
    else
        targetRotation = initialRotation;
    
    // 디버그 메시지
    FString stateMessage = isOpen ? TEXT("Door is now Opening") : TEXT("Door is now Closing");
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, stateMessage);
}

FVector ADoor::GetLeftPositionHandleWorldLocation() const
{
    if (!doorMesh)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Door 메시가 null입니다."));
        return GetActorLocation();
    }

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

// 수동으로 문 토글을 시도하는 함수 (디버깅용)
void ADoor::TryToggleDoor()
{
    // 플레이어 위치 확인
    if (playerCharacter)
    {
        float distToPlayer = FVector::Dist(GetActorLocation(), playerCharacter->GetActorLocation());
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
            FString::Printf(TEXT("Door: 플레이어까지의 거리: %.2f"), distToPlayer));
        
        // 거리가 가까운 경우 강제로 상호작용 허용
        if (distToPlayer < 300.0f)
        {
            isPlayerInRange = true;
            ToggleDoor();
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
                TEXT("Door toggle failed: Player is too far away"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
            TEXT("Door toggle failed: No player character reference"));
    }
}