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

protected:
    virtual void BeginPlay() override;
};
