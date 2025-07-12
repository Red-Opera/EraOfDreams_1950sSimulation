#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "UI/Inventory/InventorySlot.h"
#include "Inventory.generated.h"

// 게임 내 아이템 데이터를 정의하는 구조체
USTRUCT(BlueprintType)
struct FInventoryItemData
{
    GENERATED_BODY()

    // 아이템의 시각적 표현
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* itemIcon;

    // 아이템의 식별 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString itemName;

    // 보유 중인 아이템 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 count;

    // 아이템의 분류 카테고리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString itemType;

    // 아이템에 대한 상세 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString itemDescription;

    // 기본 생성자로 초기화
    FInventoryItemData()
    {
        itemIcon = nullptr;
        itemName = TEXT("");
        count = 0;
        itemType = TEXT("");
        itemDescription = TEXT("");
    }
};

// 인벤토리 UI 위젯 클래스
UCLASS()
class ERAOFDREAMS_API UInventory : public UUserWidget
{
    GENERATED_BODY()

public:
    // 생성자
    UInventory(const FObjectInitializer& ObjectInitializer);

    // 슬롯 간 수평 간격 설정 함수
    void SetSlotHorizontalSpacing(float newSpacing) { slotHorizontalSpacing = newSpacing; }
    
    // 슬롯 간 수직 간격 설정 함수
    void SetSlotVerticalSpacing(float newSpacing) { slotVerticalSpacing = newSpacing; }

protected:
    // 위젯 초기 구성 시 호출
    virtual void NativeConstruct() override;

    // 인벤토리 슬롯에 사용할 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config", meta = (DisplayName = "인벤토리 슬롯 클래스"))
    TSubclassOf<UInventorySlot> inventorySlotClass;
    
    // 각 인벤토리 슬롯의 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config", meta = (DisplayName = "슬롯 크기(픽셀)"))
    float slotSize = 100.0f;

    // 슬롯 간 가로 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config", meta = (DisplayName = "슬롯 수평 간격(픽셀)"))
    float slotHorizontalSpacing = 10.0f;

    // 슬롯 간 세로 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config", meta = (DisplayName = "슬롯 수직 간격(픽셀)"))
    float slotVerticalSpacing = 20.0f;

    // 1행 슬롯 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox1;

    // 2행 슬롯 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox2;

    // 3행 슬롯 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox3;

    // 4행 슬롯 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox4;

    // 5행 슬롯 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* slotHorizontalBox5;

    // 인벤토리 상단 구분선
    UPROPERTY(meta = (BindWidget))
    class UImage* inventoryTopLineImage;

    // 체력 상태 아이콘
    UPROPERTY(meta = (BindWidget))
    class UImage* heartSignalImage;

    // 플레이어 체력 상태 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* healthStatusText;

    // 조작 가이드 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* guideHorizontalBox;

    // 확인 버튼 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* confirmText;

    // 뒤로가기 버튼 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* backText;

    // 선택된 아이템 이름 표시
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* hoveredItemNameText;

    // 선택된 아이템 상세 정보 표시
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* hoveredItemMoreInfo;

    // 슬롯 행 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* list;

private:
    // 인벤토리에 보관 중인 아이템 목록
    UPROPERTY()
    TArray<FInventoryItemData> inventoryItems;

    // 현재 선택한 슬롯 번호
    UPROPERTY()
    int32 currentSelectedSlotIndex;

    // 생성된 인벤토리 슬롯 위젯 목록
    UPROPERTY()
    TArray<UInventorySlot*> inventorySlots;

    // 상단 단축키 클릭 처리
    UFUNCTION()
    void OnTopShortcutClicked();

    // 우측 단축키 클릭 처리
    UFUNCTION()
    void OnRightShortcutClicked();

    // 하단 단축키 클릭 처리
    UFUNCTION()
    void OnBotShortcutClicked();

    // 좌측 단축키 클릭 처리
    UFUNCTION()
    void OnLeftShortcutClicked();
    
    // 인벤토리 슬롯 클릭 이벤트 처리
    UFUNCTION()
    void OnInventorySlotClicked(int32 slotIndex);

    // 슬롯 클릭 처리 로직
    void HandleSlotClicked(int32 slotIndex);
    
    // 인벤토리 슬롯 초기 생성 및 설정
    void InitializeInventorySlots();

    // 인벤토리 UI 상태 갱신
    void UpdateInventoryDisplay();

    // 선택된 아이템 정보창 갱신
    void UpdateSelectedItemInfo(int32 SlotIndex);

    // 플레이어 체력 상태 갱신
    void UpdateHealthStatus();

    // 위젯에서 슬롯 인덱스 계산
    int32 CalculateSlotIndex(UWidget* SlotWidget);
    
    // 인벤토리 슬롯 위젯 생성
    UInventorySlot* CreateInventorySlot(int32 slotIndex);

public:
    // 인벤토리에 새 아이템 추가
    UFUNCTION(BlueprintCallable)
    bool AddItem(const FInventoryItemData& newItem);

    // 지정된 슬롯의 아이템 제거
    UFUNCTION(BlueprintCallable)
    bool RemoveItem(int32 slotIndex);

    // 아이템 사용 처리
    UFUNCTION(BlueprintCallable)
    bool UseItem(int32 slotIndex);

    // 인벤토리 UI 표시
    UFUNCTION(BlueprintCallable)
    void ShowInventory();

    // 인벤토리 UI 숨김
    UFUNCTION(BlueprintCallable)
    void HideInventory();

    // 현재 선택된 아이템 정보 반환
    UFUNCTION(BlueprintCallable)
    FInventoryItemData GetCurrentSelectedItem() const;

    // 플레이어 체력 상태 텍스트 설정
    UFUNCTION(BlueprintCallable)
    void SetHealthStatus(const FString& healthText);
    
    // 인벤토리 슬롯 위젯 클래스 설정
    UFUNCTION(BlueprintCallable)
    void SetInventorySlotClass(TSubclassOf<UInventorySlot> slotClass) { inventorySlotClass = slotClass; }
};
