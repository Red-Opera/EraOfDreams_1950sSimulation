#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundBase.h"
#include "ParkoruAudioBank.generated.h"

/**
 * 게임플레이 태그를 키로 사용하여 관련 사운드를 매핑하는 오디오 뱅크 시스템
 * 각 움직임이나 이벤트에 맞는 사운드를 쉽게 검색할 수 있습니다.
 */
UCLASS(BlueprintType)
class ERAOFDREAMS_API UParkoruAudioBank : public UPrimaryDataAsset
{
    GENERATED_BODY()
    
public:
    UParkoruAudioBank();

    // 태그로 사운드 찾기
    UFUNCTION(BlueprintCallable, Category = "Audio")
    USoundBase* FindSound(const FGameplayTag& eventTag, bool& success) const;

    // 이름으로 사운드 찾기 (문자열 입력용)
    UFUNCTION(BlueprintCallable, Category = "Audio")
    USoundBase* FindSoundByName(FName eventName, bool& success) const;

    // 사운드 추가/설정
    UFUNCTION(BlueprintCallable, Category = "Audio|Edit")
    void SetSound(const FGameplayTag& eventTag, USoundBase* sound);

    // 사운드 데이터 가져오기
    UFUNCTION(BlueprintCallable, Category = "Audio")
    const TMap<FGameplayTag, USoundBase*>& GetSoundMap() const { return soundMap; }

private:
    // 게임플레이 태그에 매핑된 사운드 (에디터에서 편집 가능)
    UPROPERTY(EditAnywhere, Category = "Sound Mapping", meta = (ForceInlineRow))
    TMap<FGameplayTag, USoundBase*> soundMap;
};
