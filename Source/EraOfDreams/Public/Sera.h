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

    UFUNCTION(BlueprintCallable, Category = "Sera")
    bool GetIsRun();

    UFUNCTION(BlueprintCallable, Category = "Sera")
    void SetIsRun(bool run) { isRun = run; }

	UFUNCTION(BlueprintCallable, Category = "Sera")
    float GetForwardSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Sera")
    float GetRightSpeed() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sera")
    bool isRun;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sera")
    float forwardAxisSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sera")
    float rightAxisSpeed;

protected:
    virtual void BeginPlay() override;

private:
    void StartRun();
    void StopRun();
	void MoveForward(float value);
    
    // 매개변수 없는 함수 추가
    void SetIsRunTrue();
    void SetIsRunFalse();
};
