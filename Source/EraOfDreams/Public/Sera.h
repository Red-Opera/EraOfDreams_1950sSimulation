#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sera.generated.h"

UCLASS()
class ERAOFDREAMS_API ASera : public ACharacter
{
    GENERATED_BODY()

public:
    ASera();

    virtual void Tick(float deltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;

    // 캐릭터를 앞으로 이동시키는 함수
    UFUNCTION(BlueprintCallable, Category = "Sera")
    void MoveForward(float value);

    // 캐릭터를 좌우로 이동시키는 함수
    UFUNCTION(BlueprintCallable, Category = "Sera")
    void MoveRight(float value);

    // 캐릭터의 달리기 상태를 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Sera")
    bool GetIsRun();

    // 캐릭터의 달리기 상태를 설정하는 함수
    UFUNCTION(BlueprintCallable, Category = "Sera")
    void SetIsRun(bool run) { isRun = run; }

    // 앞으로 이동 속도 계산 함수
    UFUNCTION(BlueprintCallable, Category = "Sera")
    float GetForwardSpeed() const;

    // 좌우 이동 속도 계산 함수
    UFUNCTION(BlueprintCallable, Category = "Sera")
    float GetRightSpeed() const;
    
    // 문 상호작용 시 이동 시작 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|Door")
    void StartDoorInteractionMovement(float speed = 0.5f, float sideMovement = 0.0f);
    
    // 문 상호작용 시 이동 종료 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|Door")
    void StopDoorInteractionMovement();
    
    // 문 상호작용 중인지 확인하는 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|Door")
    bool IsInteractingWithDoor() const { return isDoorInteraction; }

    // 캐릭터 달리기 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera")
    bool isRun;

    // 앞으로 이동 입력값
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sera")
    float forwardAxisSpeed;

    // 좌우 이동 입력값
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sera")
    float rightAxisSpeed;
    
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
    void StartRun();        // 달리기 시작 메소드
    void StopRun();         // 달리기 종료 메소드
       
    void SetIsRunTrue();    // 달리기 상태를 true로 설정하는 메소드
    void SetIsRunFalse();   // 달리기 상태를 false로 설정하는 메소드
    
    // 문 상호작용 전 이동 속도 (복원용)
    float previousForwardSpeed;
    float previousRightSpeed;
};
