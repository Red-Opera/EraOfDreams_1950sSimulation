#include "CharacterBase.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

ACharacterBase::ACharacterBase()
{
    // 기본 생성자 구현
    PrimaryActorTick.bCanEverTick = true;
}