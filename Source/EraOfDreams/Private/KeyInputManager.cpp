#include "KeyInputManager.h"

// 기존 맵을 eventPressed 맵으로 이름 변경 및 새 맵 추가
TMap<FKey, InputEvent> KeyInputManager::keyPressedEvent;
TMap<FKey, InputEvent> KeyInputManager::keyReleasedEvent;
TMap<FKey, InputEvent> KeyInputManager::keyRepeatEvent;
TMap<FKey, InputEvent> KeyInputManager::keyDoubleClickEvent;

TSet<FKey> KeyInputManager::currentPressedKeys;
TSet<FKey> KeyInputManager::previousPressedKeys;
TSet<FKey> KeyInputManager::keysPressedThisFrame;
TSet<FKey> KeyInputManager::keysReleasedThisFrame;

KeyInputManager::KeyInputManager()
{

}

KeyInputManager::~KeyInputManager()
{
    
}

void KeyInputManager::_AddEvent(const FKey& key, const InputEvent::FDelegate& delegate, EKeyEventType eventType)
{
    switch (eventType)
    {
    case EKeyEventType::Pressed:
        keyPressedEvent.FindOrAdd(key).Add(delegate);
        break;

    case EKeyEventType::Released:
        keyReleasedEvent.FindOrAdd(key).Add(delegate);
        break;

    case EKeyEventType::Repeat:
        keyRepeatEvent.FindOrAdd(key).Add(delegate);
        break;

    case EKeyEventType::DoubleClick:
        keyDoubleClickEvent.FindOrAdd(key).Add(delegate);
        break;
    }
}

void KeyInputManager::AddEvent(const FKey& key, void(*func)(), EKeyEventType eventType)
{
    KeyInputManager::_AddEvent(key, InputEvent::FDelegate::CreateStatic(func), eventType);
}

void KeyInputManager::KeyDown(const FKey& key)
{
    // 이벤트 브로드캐스트
    if (auto* delegate = keyPressedEvent.Find(key))
        delegate->Broadcast();
    
    // 키 상태 업데이트
    if (!currentPressedKeys.Contains(key))
    {
        currentPressedKeys.Add(key);
        keysPressedThisFrame.Add(key);
    }
}

void KeyInputManager::KeyUp(const FKey& key)
{
    // 이벤트 브로드캐스트
    if (auto* delegate = keyReleasedEvent.Find(key))
        delegate->Broadcast();
    
    if (currentPressedKeys.Contains(key))
    {
        currentPressedKeys.Remove(key);
        keysReleasedThisFrame.Add(key);
    }
}

void KeyInputManager::KeyRepeat(const FKey& key)
{
    // 이벤트 브로드캐스트
    if (auto* delegate = keyRepeatEvent.Find(key))
        delegate->Broadcast();
}

void KeyInputManager::KeyDoubleClick(const FKey& key)
{
    // 이벤트 브로드캐스트
    if (auto* delegate = keyDoubleClickEvent.Find(key))
        delegate->Broadcast();
}

void KeyInputManager::ProcessKeyEvent(const FKey& key, EInputEvent eventType)
{
    switch (eventType)
    {
    case IE_Pressed:
        KeyDown(key);
        break;

    case IE_Released:
        KeyUp(key);
        break;

    case IE_Repeat:
        KeyRepeat(key);
        break;

    case IE_DoubleClick:
        KeyDoubleClick(key);
        break;
    }
}

bool KeyInputManager::IsDown(const FKey& key)
{
    return currentPressedKeys.Contains(key);
}

bool KeyInputManager::IsKeyDown(const FKey& key)
{
    return keysPressedThisFrame.Contains(key);
}

bool KeyInputManager::IsKeyUp(const FKey& key)
{
    return keysReleasedThisFrame.Contains(key);
}

void KeyInputManager::Update()
{
    // 이전 프레임 데이터 저장
    previousPressedKeys = currentPressedKeys;
    
    // 이번 프레임 임시 데이터 클리어
    keysPressedThisFrame.Empty();
    keysReleasedThisFrame.Empty();
}
