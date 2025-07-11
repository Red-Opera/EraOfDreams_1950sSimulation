#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Engine/Texture2D.h"
#include "Inventory.generated.h"

// 인벤토리 아이템 구조체
USTRUCT(BlueprintType)
struct FInventoryItemData
{
    GENERATED_BODY()

    // 아이템 아이콘 이미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* itemIcon;

    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString itemName;

    // 아이템 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 quantity;

    // 아이템 타입 (무기, 의료용품 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString itemType;

    // 아이템 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString itemDescription;

    FInventoryItemData()
    {
        itemIcon = nullptr;
        itemName = TEXT("");
        quantity = 0;
        itemType = TEXT("");
        itemDescription = TEXT("");
    }
};

UCLASS()
class ERAOFDREAMS_API UInventory : public UUserWidget
{
    GENERATED_BODY()

public:
    UInventory(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    
    // 인벤토리 아이템 슬롯들 (HBBox1~5)
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox1;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox2;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox3;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox4;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox5;

    // 인벤토리 상단 라인 이미지
    UPROPERTY(meta = (BindWidget))
    class UImage* inventoryTopLineImage;

    // 체력 신호 이미지
    UPROPERTY(meta = (BindWidget))
    class UImage* heartSignalImage;

    // 체력 상태 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* healthStatusText;

    // 가이드 수평 박스
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* guideHorizontalBox;

    // 확인/뒤로가기 버튼들의 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* confirmText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* backText;

    // 호버된 아이템 이름 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* hoveredItemNameText;

    // 호버된 아이템 정보 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* hoveredItemMoreInfo;

    // 리스트 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* list;

private:
    // 인벤토리 데이터
    UPROPERTY()
    TArray<FInventoryItemData> inventoryItems;

    // 현재 선택된 슬롯 인덱스
    UPROPERTY()
    int32 currentSelectedSlotIndex;

    // 인벤토리 슬롯 버튼들 배열
    UPROPERTY()
    TArray<UButton*> inventorySlotButtons;

    // 슬롯 이미지들 배열
    UPROPERTY()
    TArray<UImage*> inventorySlotImages;

    // 슬롯 텍스트들 배열 (수량 표시용)
    UPROPERTY()
    TArray<UTextBlock*> inventorySlotTexts;

    // === 입력 처리 함수들 ===
    UFUNCTION()
    void OnTopShortcutClicked();

    UFUNCTION()
    void OnRightShortcutClicked();

    UFUNCTION()
    void OnBotShortcutClicked();

    UFUNCTION()
    void OnLeftShortcutClicked();

    // 실제 슬롯 클릭 처리 헬퍼 함수
    void HandleSlotClicked(int32 slotIndex);
    
    // 인벤토리 슬롯 초기화
    void InitializeInventorySlots();

    // 인벤토리 표시 업데이트
    void UpdateInventoryDisplay();

    // 선택된 아이템 정보 업데이트
    void UpdateSelectedItemInfo(int32 SlotIndex);

    // 체력 상태 업데이트
    void UpdateHealthStatus();

    // 슬롯 인덱스 계산 (HBBox 기반)
    int32 CalculateSlotIndex(UWidget* SlotWidget);

public:
    UFUNCTION(BlueprintCallable)
    void OnSlot0Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot1Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot2Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot3Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot4Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot5Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot6Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot7Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot8Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot9Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot10Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot11Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot12Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot13Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot14Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot15Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot16Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot17Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot18Clicked();
    
    UFUNCTION(BlueprintCallable)
    void OnSlot19Clicked();

    // 아이템 추가
    UFUNCTION(BlueprintCallable)
    bool AddItem(const FInventoryItemData& newItem);

    // 아이템 제거
    UFUNCTION(BlueprintCallable)
    bool RemoveItem(int32 slotIndex);

    // 아이템 사용
    UFUNCTION(BlueprintCallable)
    bool UseItem(int32 slotIndex);

    // 인벤토리 표시
    UFUNCTION(BlueprintCallable)
    void ShowInventory();

    // 인벤토리 숨김
    UFUNCTION(BlueprintCallable)
    void HideInventory();

    // 현재 선택된 아이템 가져오기
    UFUNCTION(BlueprintCallable)
    FInventoryItemData GetCurrentSelectedItem() const;

    // 체력 상태 설정
    UFUNCTION(BlueprintCallable)
    void SetHealthStatus(const FString& healthText);
};
