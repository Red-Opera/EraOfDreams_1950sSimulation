// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicGame.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMusicGame::AMusicGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Start Box Collider Component"));

	SetRootComponent(boxComponent);
}

// Called when the game starts or when spawned
void AMusicGame::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AMusicGame::OnOverlapBegin(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (otherActor == nullptr || player == nullptr)
		return;

	ACharacter* collisionActor = Cast<ACharacter>(otherActor);

	if (collisionActor != nullptr && player == collisionActor)
		UE_LOG(LogTemp, Warning, TEXT("SDFF"));
}

// Called every frame
void AMusicGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}