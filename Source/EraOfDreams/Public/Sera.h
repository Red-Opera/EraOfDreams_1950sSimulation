#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Sera.generated.h"

UCLASS()
class ERAOFDREAMS_API ASera : public ACharacterBase
{
    GENERATED_BODY()

public:
    ASera();

    virtual void Tick(float deltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;
    
    // 문 상호작용 시 이동 시작 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|Door")
    void StartDoorInteractionMovement(float speed = 0.5f, float sideMovement = 0.0f);
    
    // 문 상호작용 시 이동 종료 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|Door")
    void StopDoorInteractionMovement();
    
    // 문 상호작용 중인지 확인하는 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|Door")
    bool IsInteractingWithDoor() const { return isDoorInteraction; }
    
    // 문 상호작용 상태 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera|Door")
    bool isDoorInteraction;
    
    // 문 상호작용 시 사용할 이동 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera|Door")
    float doorInteractionSpeed;
    
    // 문 상호작용 시 사용할 좌우 이동 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera|Door")
    float doorInteractionSideSpeed;
    
    // 문 상호작용 일시 정지 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera|Door")
    bool isPaused;
    
    // 문 상호작용 중 플레이어가 이동을 제어할 수 있는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera|Door")
    bool isControlMovement;

protected:
    virtual void BeginPlay() override;

private:
    // 문 상호작용 전 이동 상태 (복원용)
    FVector2D previousMovementInput;
    
    // 문 상호작용 전 달리기 상태
    bool wasRunning;
};
