#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SeraController.generated.h"

UCLASS()
class ERAOFDREAMS_API ASeraController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void OnAnyKeyPressed(FKey Key);		// 어떤 키가 눌렸는지 감지해 처리하는 메소드
	void OnAnyKeyReleased(FKey Key);	// 어떤 키가 떼어졌는지 감지해 처리하는 메소드
};
