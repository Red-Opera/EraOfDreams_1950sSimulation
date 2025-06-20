#include "Door.h"
#include "KeyInputManager.h"
#include "Sera.h"
#include "SeraAnimation.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 메시 컴포넌트 생성
    doorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = doorMesh;

    // 상호작용 박스 컴포넌트 생성
    interactionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    interactionBox->SetupAttachment(RootComponent);
    interactionBox->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f));

    // 충돌 설정
    interactionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    interactionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    interactionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    interactionBox->SetGenerateOverlapEvents(true);

    // 오버랩 이벤트 바인딩
    interactionBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnInteractionBeginOverlap);
    interactionBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnInteractionEndOverlap);
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

    // G키 이벤트 등록
    KeyInputManager::AddEvent(EKeys::G, this, &ADoor::ToggleDoor);
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
}

void ADoor::OnInteractionBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
    UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& sweepResult)
{
    if (otherActor == nullptr) return;

    ASera* seraPlayer = Cast<ASera>(otherActor);
    if (seraPlayer != nullptr && seraPlayer == playerCharacter)
    {
        isPlayerInRange = true;
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Press G to interact with door"));
    }
}

void ADoor::OnInteractionEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
    UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
    if (otherActor == nullptr) return;

    ASera* seraPlayer = Cast<ASera>(otherActor);
    if (seraPlayer == nullptr) return;

    if (seraPlayer == playerCharacter)
        isPlayerInRange = false;
}

void ADoor::ToggleDoor()
{
    // 플레이어가 범위 내에 있고 문이 움직이고 있지 않을 때만 토글
    if (isPlayerInRange && !isMoving && playerCharacter)
    {
        isMoving = true;
        isOpen = !isOpen;

        // 문 열기/닫기
        if (isOpen)
            targetRotation = initialRotation + FRotator(0.0f, openAngle, 0.0f);
        else
            targetRotation = initialRotation;

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
                    seraAnimInstance->OnDoorInteractionStarted(leftHandleLocation);
                }
            }
        }

        // 디버그 메시지 
        FString stateMessage = isOpen ? TEXT("Opening Door") : TEXT("Closing Door");
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, stateMessage);
        
        FString locationMessage = FString::Printf(TEXT("LeftPositionHandle Location: %s"), *leftHandleLocation.ToString());
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, locationMessage);
    }
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