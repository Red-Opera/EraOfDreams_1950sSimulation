// Copyright (c) 2024 Marvin Bernd Watanabe

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameAnimationBasePlayerControllerInterface.generated.h"

UINTERFACE()
class UGameAnimationBasePlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class ERAOFDREAMS_API IGameAnimationBasePlayerControllerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Input|Action")
		bool IsFullMovementInput() const;
};
