#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MoveParkoruAsset/ParkoruAudioBank.h"
#include "ParkoruAudioBankInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UParkoruAudioBankInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 파쿠르 오디오 시스템과 관련된 기능을 제공하는 인터페이스
 * 발소리 재생 가능 여부와 Foley 오디오 뱅크 접근 기능을 제공합니다.
 */
class ERAOFDREAMS_API IParkoruAudioBankInterface
{
    GENERATED_BODY()

public:
    // 발소리를 재생할 수 있는지 확인
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Audio|Parkour")
    bool CanPlayFootstepSounds() const;

    // Foley 오디오 뱅크 가져오기
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Audio|Parkour")
    UParkoruAudioBank* GetMoveParkoruAudioBank() const;
};
