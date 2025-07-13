#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"

// 키 이벤트용 멀티캐스트 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(InputEvent);

// 입력 이벤트 타입 열거형 (언리얼 엔진의 EInputEvent와 일치)
enum class EKeyEventType : uint8
{
    Pressed,    // IE_Pressed
    Released,   // IE_Released
    Repeat,     // IE_Repeat
    DoubleClick // IE_DoubleClick
};

class ERAOFDREAMS_API KeyInputManager
{
public:
    KeyInputManager();
    ~KeyInputManager();

    // 키 이벤트 추가 메소드 (템플릿 버전)
    template<typename UObjectType>
    static void AddEvent(const FKey& key, UObjectType* object, void (UObjectType::*func)(), EKeyEventType eventType = EKeyEventType::Pressed)
    {
        _AddEvent(key, InputEvent::FDelegate::CreateUObject(object, func), eventType);
    }

    // 키 이벤트 추가 메소드 (함수 포인터 버전)
    static void AddEvent(const FKey& key, void(*func)(), EKeyEventType eventType = EKeyEventType::Pressed);

    // 키 이벤트 처리 메소드들
    static void KeyDown(const FKey& key);
    static void KeyUp(const FKey& key);
    static void KeyRepeat(const FKey& key);
    static void KeyDoubleClick(const FKey& key);

    // 언리얼 EInputEvent를 내부 EKeyEventType으로 변환
    static void ProcessKeyEvent(const FKey& key, EInputEvent eventType);
    
    // 키 상태 확인 메소드들
    static bool IsDown(const FKey& key);            // 현재 눌려있는지
    static bool IsKeyDown(const FKey& key);         // 이번 프레임에 눌렸는지
    static bool IsKeyUp(const FKey& key);           // 이번 프레임에 떼어졌는지
    
    // 매 프레임 호출해야 하는 업데이트 함수
    static void Update();

    // 키 별로 바인딩된 델리케이트를 모아두는 맵 (이벤트 타입별)
    static TMap<FKey, InputEvent> keyPressedEvent;
    static TMap<FKey, InputEvent> keyReleasedEvent;
    static TMap<FKey, InputEvent> keyRepeatEvent;
    static TMap<FKey, InputEvent> keyDoubleClickEvent;

private:
    static void _AddEvent(const FKey& key, const InputEvent::FDelegate& delegate, EKeyEventType eventType);
    
    // 키 상태 추적용
    static TSet<FKey> currentPressedKeys;		// 현재 눌려있는 키들
    static TSet<FKey> previousPressedKeys;		// 이전 프레임에 눌려있던 키들
    static TSet<FKey> keysPressedThisFrame;	    // 이번 프레임에 눌린 키들
    static TSet<FKey> keysReleasedThisFrame;    // 이번 프레임에 떼어진 키들
};