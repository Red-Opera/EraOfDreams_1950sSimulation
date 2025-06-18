#include "EraOfDreamsGameMode.h"
#include "EraOfDreamsCharacter.h"
#include "KeyInputManager.h"

#include "UObject/ConstructorHelpers.h"

AEraOfDreamsGameMode::AEraOfDreamsGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    // GameMode도 Tick이 가능하도록 설정
    PrimaryActorTick.bCanEverTick = true;
    
    // 모든 액터의 Tick이 끝난 후에 실행되도록 설정
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
}

void AEraOfDreamsGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AEraOfDreamsGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 모든 액터의 Tick이 끝난 후 KeyInputManager 업데이트
    KeyInputManager::Update();
}
