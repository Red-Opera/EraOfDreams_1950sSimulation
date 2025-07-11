#include "UI/Inventory/Inventory.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"

UInventory::UInventory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 초기값 설정
    currentSelectedSlotIndex = -1;
}

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();

    // 인벤토리 슬롯 초기화
    InitializeInventorySlots();

    // 체력 상태 초기화
    UpdateHealthStatus();

    // 확인/뒤로가기 텍스트 설정
    if (confirmText != nullptr)
        confirmText->SetText(FText::FromString(TEXT("Confirm")));

    if (backText != nullptr)
        backText->SetText(FText::FromString(TEXT("Back")));

    // 체력 상태 텍스트 초기 설정
    SetHealthStatus(TEXT("FINE"));

    // 디버그 메시지
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("[Inventory] 인벤토리 위젯 초기화 완료"));
}

void UInventory::InitializeInventorySlots()
{
    // 슬롯 배열 초기화
    inventorySlotButtons.Empty();
    inventorySlotImages.Empty();
    inventorySlotTexts.Empty();

    // HBBox들을 배열로 수집
    TArray<UHorizontalBox*> slotHorizontalBoxes = 
    { 
        slotHorizontalBox1, 
        slotHorizontalBox2, 
        slotHorizontalBox3, 
        slotHorizontalBox4, 
        slotHorizontalBox5 
    };

    // 각 HBBox에 4개씩 슬롯 생성 (총 20개 슬롯)
    for (int32 boxIndex = 0; boxIndex < slotHorizontalBoxes.Num(); ++boxIndex)
    {
        UHorizontalBox* currentHBox = slotHorizontalBoxes[boxIndex];

        if (!currentHBox)
            continue;

        // 기존 자식 위젯들 제거
        currentHBox->ClearChildren();

        // 4개의 슬롯 생성
        for (int32 slotIndex = 0; slotIndex < 4; ++slotIndex)
        {
            // 슬롯 버튼 생성
            UButton* slotButton = WidgetTree->ConstructWidget<UButton>();
            
            // 슬롯 테두리 생성
            UBorder* slotBorder = WidgetTree->ConstructWidget<UBorder>();
            
            // 슬롯 이미지 생성
            UImage* slotImage = WidgetTree->ConstructWidget<UImage>();
            
            // 슬롯 수량 텍스트 생성
            UTextBlock* slotText = WidgetTree->ConstructWidget<UTextBlock>();

            if (slotButton && slotBorder && slotImage && slotText)
            {
                // 슬롯 구조 설정: Button > Border > Image + Text
                slotButton->AddChild(slotBorder);
                slotBorder->AddChild(slotImage);
                slotBorder->AddChild(slotText);

                // 슬롯 스타일 설정
                slotBorder->SetPadding(FMargin(2.0f));
                
                // 수량 텍스트 설정
                slotText->SetText(FText::FromString(TEXT("")));
                slotText->SetJustification(ETextJustify::Right);

                // HBBox에 슬롯 추가
                currentHBox->AddChild(slotButton);

                // 배열에 추가
                inventorySlotButtons.Add(slotButton);
                inventorySlotImages.Add(slotImage);
                inventorySlotTexts.Add(slotText);

                // 각 버튼에 해당하는 슬롯 인덱스 계산
                int32 buttonIndex = boxIndex * 4 + slotIndex;
                
                // 슬롯 인덱스에 따라 적절한 함수 바인딩
                switch (buttonIndex)
                {
                case 0: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot0Clicked); break;
                case 1: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot1Clicked); break;
                case 2: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot2Clicked); break;
                case 3: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot3Clicked); break;
                case 4: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot4Clicked); break;
                case 5: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot5Clicked); break;
                case 6: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot6Clicked); break;
                case 7: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot7Clicked); break;
                case 8: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot8Clicked); break;
                case 9: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot9Clicked); break;
                case 10: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot10Clicked); break;
                case 11: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot11Clicked); break;
                case 12: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot12Clicked); break;
                case 13: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot13Clicked); break;
                case 14: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot14Clicked); break;
                case 15: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot15Clicked); break;
                case 16: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot16Clicked); break;
                case 17: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot17Clicked); break;
                case 18: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot18Clicked); break;
                case 19: slotButton->OnClicked.AddDynamic(this, &UInventory::OnSlot19Clicked); break;
                }
                
                // 디버그용 텍스트 추가 (슬롯 번호 확인용)
                slotText->SetText(FText::FromString(FString::FromInt(buttonIndex)));
            }
        }
    }

    // 디버그 메시지
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
        FString::Printf(TEXT("[Inventory] 인벤토리 슬롯 %d개 생성 완료"), inventorySlotButtons.Num()));
}

void UInventory::UpdateInventoryDisplay()
{
    // 모든 슬롯 초기화
    for (int32 i = 0; i < inventorySlotImages.Num(); ++i)
    {
        if (inventorySlotImages[i])
            inventorySlotImages[i]->SetBrushFromTexture(nullptr);
        
        if (inventorySlotTexts[i])
            inventorySlotTexts[i]->SetText(FText::FromString(TEXT("")));
    }

    // 아이템 데이터로 슬롯 업데이트
    for (int32 i = 0; i < inventoryItems.Num() && i < inventorySlotImages.Num(); ++i)
    {
        const FInventoryItemData& item = inventoryItems[i];
        
        // 아이템 아이콘 설정
        if (inventorySlotImages[i] && item.itemIcon)
            inventorySlotImages[i]->SetBrushFromTexture(item.itemIcon);
        
        // 아이템 수량 텍스트 설정
        if (inventorySlotTexts[i] && item.quantity > 0)
            inventorySlotTexts[i]->SetText(FText::FromString(FString::FromInt(item.quantity)));
    }
}

void UInventory::UpdateSelectedItemInfo(int32 slotIndex)
{
    if (slotIndex >= 0 && slotIndex < inventoryItems.Num())
    {
        const FInventoryItemData& item = inventoryItems[slotIndex];
        
        // 호버된 아이템 이름 설정
        if (hoveredItemNameText != nullptr)
            hoveredItemNameText->SetText(FText::FromString(item.itemName));
        
        // 호버된 아이템 추가 정보 설정
        if (hoveredItemMoreInfo != nullptr)
        {
            FString infoText = FString::Printf(TEXT("[Inventory] %s\n수량: %d\n%s"), *item.itemType, item.quantity, *item.itemDescription);
            hoveredItemMoreInfo->SetText(FText::FromString(infoText));
        }
    }
    else
    {
        // 선택된 아이템이 없을 때 초기화
        if (hoveredItemNameText != nullptr)
            hoveredItemNameText->SetText(FText::FromString(TEXT("")));
        
        if (hoveredItemMoreInfo != nullptr)
            hoveredItemMoreInfo->SetText(FText::FromString(TEXT("")));
    }
}

void UInventory::UpdateHealthStatus()
{
    // 체력 상태는 외부에서 설정하도록 함
    // 현재는 기본값으로 "FINE" 표시
    if (healthStatusText != nullptr)
        healthStatusText->SetText(FText::FromString(TEXT("FINE")));
}

int32 UInventory::CalculateSlotIndex(UWidget* SlotWidget)
{
    UButton* clickedButton = Cast<UButton>(SlotWidget);

    if (clickedButton == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 클릭된 위젯이 버튼이 아닙니다."));
        return -1;
    }

    // 버튼 배열에서 인덱스 찾기
    return inventorySlotButtons.Find(clickedButton);
}

// === 입력 처리 함수들 ===

void UInventory::OnTopShortcutClicked()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 상단 단축키 클릭됨"));
}

void UInventory::OnRightShortcutClicked()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 우측 단축키 클릭됨"));
}

void UInventory::OnBotShortcutClicked()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 하단 단축키 클릭됨"));
}

void UInventory::OnLeftShortcutClicked()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 좌측 단축키 클릭됨"));
}

void UInventory::HandleSlotClicked(int32 slotIndex)
{
    // 슬롯 인덱스 유효성 검사
    if (slotIndex >= 0 && slotIndex < inventorySlotButtons.Num())
    {
		// 클릭된 슬롯의 인덱스 계산
        currentSelectedSlotIndex = slotIndex;
        UpdateSelectedItemInfo(slotIndex);

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("[Inventory] 인벤토리 슬롯 클릭 감지됨 - 슬롯: %d"), slotIndex));
    }

    // 슬롯이 유효하지 않은 경우
    else
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("[Inventory] 슬롯 %d: 유효하지 않은 슬롯입니다"), slotIndex));
}

// 블루프린트에서 호출할 수 있는 개별 슬롯 클릭 함수들 수정
void UInventory::OnSlot0Clicked() { HandleSlotClicked(0); }
void UInventory::OnSlot1Clicked() { HandleSlotClicked(1); }
void UInventory::OnSlot2Clicked() { HandleSlotClicked(2); }
void UInventory::OnSlot3Clicked() { HandleSlotClicked(3); }
void UInventory::OnSlot4Clicked() { HandleSlotClicked(4); }
void UInventory::OnSlot5Clicked() { HandleSlotClicked(5); }
void UInventory::OnSlot6Clicked() { HandleSlotClicked(6); }
void UInventory::OnSlot7Clicked() { HandleSlotClicked(7); }
void UInventory::OnSlot8Clicked() { HandleSlotClicked(8); }
void UInventory::OnSlot9Clicked() { HandleSlotClicked(9); }
void UInventory::OnSlot10Clicked() { HandleSlotClicked(10); }
void UInventory::OnSlot11Clicked() { HandleSlotClicked(11); }
void UInventory::OnSlot12Clicked() { HandleSlotClicked(12); }
void UInventory::OnSlot13Clicked() { HandleSlotClicked(13); }
void UInventory::OnSlot14Clicked() { HandleSlotClicked(14); }
void UInventory::OnSlot15Clicked() { HandleSlotClicked(15); }
void UInventory::OnSlot16Clicked() { HandleSlotClicked(16); }
void UInventory::OnSlot17Clicked() { HandleSlotClicked(17); }
void UInventory::OnSlot18Clicked() { HandleSlotClicked(18); }
void UInventory::OnSlot19Clicked() { HandleSlotClicked(19); }

bool UInventory::AddItem(const FInventoryItemData& newItem)
{
    // 빈 슬롯 찾기
    if (inventoryItems.Num() < 20) // 최대 20개 슬롯
    {
        inventoryItems.Add(newItem);
        UpdateInventoryDisplay();
        
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("[Inventory] 아이템 추가됨: %s"), *newItem.itemName));
        
        return true;
    }
    
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 인벤토리가 가득참"));

    return false;
}

bool UInventory::RemoveItem(int32 slotIndex)
{
    if (slotIndex < 0 || slotIndex >= inventoryItems.Num())
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 유효하지 않은 슬롯 인덱스"));

        return false;
    }

    FString itemName = inventoryItems[slotIndex].itemName;

    inventoryItems.RemoveAt(slotIndex);
    UpdateInventoryDisplay();

    // 선택된 슬롯이 제거되었으면 선택 해제
    if (currentSelectedSlotIndex == slotIndex)
    {
        currentSelectedSlotIndex = -1;

        UpdateSelectedItemInfo(-1);
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("[Inventory] 아이템 제거됨: %s"), *itemName));

    return true;
}

bool UInventory::UseItem(int32 slotIndex)
{
    if (currentSelectedSlotIndex != slotIndex)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 현재 선택된 슬롯과 사용하려는 슬롯이 다릅니다."));

        return false;
    }

    FInventoryItemData& item = inventoryItems[slotIndex];

    if (item.quantity > 0)
    {
        item.quantity--;

        // 수량이 0이 되면 아이템 제거
        if (item.quantity <= 0)
            RemoveItem(slotIndex);

        else
            UpdateInventoryDisplay();

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("[Inventory] 아이템 사용됨: %s"), *item.itemName));

        return true;
    }

    return false;
}

void UInventory::ShowInventory()
{
    SetVisibility(ESlateVisibility::Visible);
    UpdateInventoryDisplay();

    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("[Inventory] 인벤토리 표시됨"));
}

void UInventory::HideInventory()
{
    SetVisibility(ESlateVisibility::Hidden);

    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("[Inventory] 인벤토리 숨겨짐"));
}

FInventoryItemData UInventory::GetCurrentSelectedItem() const
{
	// 현재 선택된 슬롯 인덱스가 유효한지 확인
    if (currentSelectedSlotIndex >= 0 && currentSelectedSlotIndex < inventoryItems.Num())
        return inventoryItems[currentSelectedSlotIndex];

    // 빈 아이템 반환
    return FInventoryItemData(); 
}

void UInventory::SetHealthStatus(const FString& healthText)
{
    if (healthStatusText)
        healthStatusText->SetText(FText::FromString(healthText));
}