#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/Inventory.h"
#include "PlayerMenu.generated.h"

UCLASS()
class ERAOFDREAMS_API UPlayerMenu : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // 인벤토리 위젯 참조 (블루프린트에서 바인딩)
    // 블루프린트에서 접근할 수 있도록 BlueprintReadOnly 추가
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UInventory* inventory;

    // 메인 입력 처리
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
    // 인벤토리 토글 함수
    UFUNCTION(BlueprintCallable)
    void ToggleInventory();

    // 인벤토리 표시 함수
    UFUNCTION(BlueprintCallable)
    void ShowInventory();

    // 인벤토리 숨김 함수
    UFUNCTION(BlueprintCallable)
    void HideInventory();

    // 테스트용 아이템 추가
    UFUNCTION(BlueprintCallable)
    void AddTestItems();

    // 인벤토리 위젯 getter 함수 추가
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventory* GetInventory() const { return inventory; }

    // 인벤토리 열림 상태 확인
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsInventoryOpen() const { return isInventoryOpen; }

private:
    // 인벤토리 열림 상태
    bool isInventoryOpen;

    // KeyInputManager를 통한 입력 처리 함수들
    void HandleInventoryToggle();
    void HandleEscapeKey();
    
    // 입력 이벤트 등록/해제
    void RegisterInputEvents();
    void UnregisterInputEvents();
};