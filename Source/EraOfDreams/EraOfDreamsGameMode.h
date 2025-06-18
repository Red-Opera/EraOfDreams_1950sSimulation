#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EraOfDreamsGameMode.generated.h"

UCLASS(minimalapi)
class AEraOfDreamsGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AEraOfDreamsGameMode();

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
};