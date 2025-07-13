#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MoveParkoru/Core/GameAnimationBasePlayerController.h"
#include "SeraController.generated.h"

UCLASS()
class ERAOFDREAMS_API ASeraController : public AGameAnimationBasePlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    void OnAnyKeyPressed(FKey Key);      // 어떤 키가 눌렸는지 감지해 처리하는 메소드
    void OnAnyKeyReleased(FKey Key);     // 어떤 키가 떼어졌는지 감지해 처리하는 메소드
    void OnAnyKeyRepeat(FKey Key);       // 어떤 키가 반복 입력되었는지 감지해 처리하는 메소드
    void OnAnyKeyDoubleClick(FKey Key);  // 어떤 키가 더블 클릭되었는지 감지해 처리하는 메소드
};
