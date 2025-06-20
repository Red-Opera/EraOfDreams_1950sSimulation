#include "KeyInputManager.h"

TMap<FKey, InputEvent> KeyInputManager::keyDownEvent;
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

void KeyInputManager::_AddEvent(const FKey& key, const InputEvent::FDelegate& delegate)
{
    keyDownEvent.FindOrAdd(key).Add(delegate);
}

void KeyInputManager::AddEvent(const FKey& key, void(*func)())
{
    KeyInputManager::_AddEvent(key, InputEvent::FDelegate::CreateStatic(func));
}

void KeyInputManager::KeyDown(const FKey& key)
{
    // 이벤트 브로드캐스트
    if (auto* delegate = keyDownEvent.Find(key))
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
    if (currentPressedKeys.Contains(key))
    {
        currentPressedKeys.Remove(key);
        keysReleasedThisFrame.Add(key);
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
