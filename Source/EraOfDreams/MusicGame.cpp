#include "MusicGame.h"
#include "KeyInputManager.h"

#include "EngineUtils.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

#include "Camera/CameraActor.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMusicGame::AMusicGame()
{
	isStart = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Start Box Collider Component"));

	SetRootComponent(boxComponent);

	// 생성자 또는 BeginPlay에서
	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMusicGame::OnCollisionPlayer);

	// 충돌 속성 설정
	boxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	boxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	boxComponent->SetGenerateOverlapEvents(true);
}

void AMusicGame::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerController = GetWorld()->GetFirstPlayerController();

	//KeyInputManager::AddEvent(EKeys::G, this, &AMusicGame::A);
}

void AMusicGame::OnCollisionPlayer(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (otherActor == nullptr || player == nullptr)
		return;

	ACharacter* collisionActor = Cast<ACharacter>(otherActor);

	if (collisionActor != nullptr && player == collisionActor)
	{
		if (!isStart)
		{
			ChangeToGameCamera();

			isStart = true;
		}
	}
}

void AMusicGame::ChangeToGameCamera()
{
	if (gameSequence == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Camera Change Fail!!"));
		return;
	}

	// Create a level sequence player
	ALevelSequenceActor* sequenceActor = nullptr;
	ULevelSequencePlayer* levelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), 
		gameSequence, 
		FMovieSceneSequencePlaybackSettings(), 
		sequenceActor);

	if (!levelSequencePlayer || !sequenceActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create Level Sequence Player or Actor."));
		return;
	}

	levelSequencePlayer->Play();
}

void AMusicGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}