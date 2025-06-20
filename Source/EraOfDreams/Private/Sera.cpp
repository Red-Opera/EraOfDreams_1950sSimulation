#include "Sera.h"
#include "SeraAnimation.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

ASera::ASera()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASera::BeginPlay()
{
    Super::BeginPlay();
}

void ASera::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void ASera::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
    Super::SetupPlayerInputComponent(playerInputComponent);
}

