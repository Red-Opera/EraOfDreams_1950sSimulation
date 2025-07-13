#include "MoveParkoru/Asset/ParkoruAudioBank.h"

UParkoruAudioBank::UParkoruAudioBank()
{
    // 기본 생성자
}

USoundBase* UParkoruAudioBank::FindSound(const FGameplayTag& eventTag, bool& success) const
{
    // 게임플레이 태그로 사운드 검색
    USoundBase* const* foundSound = soundMap.Find(eventTag);
    
    if (foundSound != nullptr && *foundSound != nullptr)
    {
        success = true;

        return *foundSound;
    }
    
    // 태그가 맵에 없거나 사운드 애셋이 유효하지 않음
    success = false;

    return nullptr;
}

USoundBase* UParkoruAudioBank::FindSoundByName(FName eventName, bool& success) const
{
    // 이름으로 게임플레이 태그 생성
    FGameplayTag eventTag = FGameplayTag::RequestGameplayTag(eventName, false);

    // 유효한 태그인 경우에만 검색
    if (eventTag.IsValid())
        return FindSound(eventTag, success);

    success = false;

    return nullptr;
}

void UParkoruAudioBank::SetSound(const FGameplayTag& eventTag, USoundBase* sound)
{
    if (eventTag.IsValid())
    {
        if (sound != nullptr)
            soundMap.Add(eventTag, sound);

        // Sound가 nullptr인 경우 맵에서 해당 태그 제거
        else
            soundMap.Remove(eventTag);
    }
}
