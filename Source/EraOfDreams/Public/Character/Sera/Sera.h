#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Sera.generated.h"

class UPlayerMenu;

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
    
    // 인벤토리 토글 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|UI")
    void ToggleInventory();
    
    // PlayerMenu 위젯 인스턴스 반환 함수
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sera|UI")
    UPlayerMenu* GetPlayerMenu() const;
    
    // PlayerMenu 위젯 생성 함수
    UFUNCTION(BlueprintCallable, Category = "Sera|UI")
    void CreatePlayerMenu();
    
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
    
    // 블루프린트에서 설정할 수 있는 PlayerMenu 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sera|UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UPlayerMenu> playerMenuClass;

private:
    // 문 상호작용 전 이동 상태 (복원용)
    FVector2D previousMovementInput;
    
    // 문 상호작용 전 달리기 상태
    bool wasRunning;
    
    // 인벤토리 키 입력 처리 함수
    void HandleInventoryToggle();
    
    // 생성된 PlayerMenu 위젯 인스턴스 (내부적으로만 사용)
    UPROPERTY()
    UPlayerMenu* createdPlayerMenu;
};
