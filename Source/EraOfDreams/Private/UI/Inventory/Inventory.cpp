#include "UI/Inventory/Inventory.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "UI/Inventory/InventorySlot.h"

UInventory::UInventory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본 값 초기화
    currentSelectedSlotIndex = -1;
    inventorySlotClass = UInventorySlot::StaticClass();
}

void UInventory::NativeConstruct()
{
    Super::NativeConstruct();

    // 인벤토리 초기 설정
    InitializeInventorySlots();
    UpdateHealthStatus();

    // UI 텍스트 초기화
    if (confirmText != nullptr)
        confirmText->SetText(FText::FromString(TEXT("Confirm")));

    if (backText != nullptr)
        backText->SetText(FText::FromString(TEXT("Back")));

    // 기본 체력 상태 설정
    SetHealthStatus(TEXT("FINE"));

    // 초기화 완료 메시지
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("[Inventory] 인벤토리 위젯 초기화 완료"));
}

UInventorySlot* UInventory::CreateInventorySlot(int32 slotIndex)
{
    // 슬롯 클래스 유효성 검사
    if (!inventorySlotClass)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
            TEXT("[Inventory] 경고: 인벤토리 슬롯 클래스가 설정되지 않았습니다. 블루프린트에서 인벤토리 슬롯 클래스를 설정하세요."));

        return nullptr;
    }
    
    // 슬롯 위젯 생성 및 설정
    UInventorySlot* newSlot = CreateWidget<UInventorySlot>(GetOwningPlayer(), inventorySlotClass);
    
    if (newSlot)
    {
        newSlot->SetSlotIndex(slotIndex);
        newSlot->OnSlotClicked.AddDynamic(this, &UInventory::OnInventorySlotClicked);
    }

    else
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[Inventory] 오류: 인벤토리 슬롯 위젯을 생성할 수 없습니다."));
    
    return newSlot;
}

void UInventory::InitializeInventorySlots()
{
    // 슬롯 클래스 유효성 검사
    if (!inventorySlotClass)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Inventory] 오류: 인벤토리 슬롯 클래스가 설정되지 않았습니다. 블루프린트에서 설정하세요."));

        return;
    }
    
    // 기존 슬롯 초기화
    inventorySlots.Empty();

    // 수평 박스 컨테이너 배열 구성
    TArray<UHorizontalBox*> slotHorizontalBoxes = 
    { 
        slotHorizontalBox1, 
        slotHorizontalBox2, 
        slotHorizontalBox3, 
        slotHorizontalBox4, 
        slotHorizontalBox5 
    };

    // 수직 간격 설정
    if (list != nullptr)
    {
        for (int32 i = 0; i < list->GetChildrenCount(); i++)
        {
            UWidget* child = list->GetChildAt(i);

            if (UHorizontalBox* hBox = Cast<UHorizontalBox>(child))
            {
                UVerticalBoxSlot* vBoxSlot = Cast<UVerticalBoxSlot>(hBox->Slot);

                if (vBoxSlot)
                    vBoxSlot->SetPadding(FMargin(0, 0, 0, slotVerticalSpacing));
            }
        }
    }

    // 각 행에 슬롯 생성 (5행 x 4열 = 20개)
    for (int32 boxIndex = 0; boxIndex < slotHorizontalBoxes.Num(); boxIndex++)
    {
        UHorizontalBox* currentHBox = slotHorizontalBoxes[boxIndex];

        if (!currentHBox)
            continue;

        // 기존 슬롯 제거
        currentHBox->ClearChildren();

        // 각 행에 4개 슬롯 생성
        for (int32 slotIndex = 0; slotIndex < 4; slotIndex++)
        {
            int32 inventoryIndex = boxIndex * 4 + slotIndex;
            
            // 크기 제한 박스 생성
            USizeBox* sizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
            sizeBox->SetWidthOverride(slotSize);
            sizeBox->SetHeightOverride(slotSize);
            
            // 슬롯 위젯 생성
            UInventorySlot* newSlot = CreateInventorySlot(inventoryIndex);
            
            if (newSlot != nullptr)
            {
                sizeBox->AddChild(newSlot);
                
                // 수평 박스에 추가 및 간격 설정
                UHorizontalBoxSlot* hBoxSlot = Cast<UHorizontalBoxSlot>(currentHBox->AddChild(sizeBox));
                
                if (hBoxSlot && slotIndex < 3)
                {
                    hBoxSlot->SetPadding(FMargin(0, 0, slotHorizontalSpacing, 0));
                }
                
                inventorySlots.Add(newSlot);
                newSlot->SetItemCount(inventoryIndex);
            }
        }
    }

    // 초기화 완료 메시지
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
        FString::Printf(TEXT("[Inventory] 인벤토리 슬롯 %d개 생성 완료 (크기: %.1f x %.1f, 간격: 가로 %.1f, 세로 %.1f)"), 
        inventorySlots.Num(), slotSize, slotSize, slotHorizontalSpacing, slotVerticalSpacing));
}

void UInventory::OnInventorySlotClicked(int32 slotIndex)
{
    // 슬롯 클릭 처리 위임
    HandleSlotClicked(slotIndex);
}

void UInventory::HandleSlotClicked(int32 slotIndex)
{
    // 슬롯 인덱스 유효성 검사
    if (slotIndex >= 0 && slotIndex < inventorySlots.Num())
    {
        // 선택 상태 갱신
        int32 oldSelectedSlot = currentSelectedSlotIndex;
        currentSelectedSlotIndex = slotIndex;
        
        // 이전 선택 슬롯 선택 해제
        if (oldSelectedSlot >= 0 && oldSelectedSlot < inventorySlots.Num())
            inventorySlots[oldSelectedSlot]->SetSelected(false);
                
        // 새 슬롯 선택
        inventorySlots[slotIndex]->SetSelected(true);
        
        // 정보창 갱신
        UpdateSelectedItemInfo(slotIndex);

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
            FString::Printf(TEXT("[Inventory] 인벤토리 슬롯 클릭 감지됨 - 슬롯: %d"), slotIndex));
    }

    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
            FString::Printf(TEXT("[Inventory] 슬롯 %d: 유효하지 않은 슬롯입니다"), slotIndex));
    }
}

void UInventory::UpdateInventoryDisplay()
{
    // 모든 슬롯 초기화
    for (int32 i = 0; i < inventorySlots.Num(); i++)
    {
        if (inventorySlots[i] == nullptr)
            continue;


        inventorySlots[i]->SetItemIcon(nullptr);
        inventorySlots[i]->SetItemCount(0);
        inventorySlots[i]->SetSelected(i == currentSelectedSlotIndex);
    }

    // 아이템 데이터로 슬롯 UI 갱신
    for (int32 i = 0; i < inventoryItems.Num() && i < inventorySlots.Num(); i++)
    {
        const FInventoryItemData& item = inventoryItems[i];

        if (inventorySlots[i] == nullptr)
            continue;

        inventorySlots[i]->SetItemIcon(item.itemIcon);
        inventorySlots[i]->SetItemCount(item.count);
        inventorySlots[i]->SetSelected(i == currentSelectedSlotIndex);
    }
}

void UInventory::UpdateSelectedItemInfo(int32 slotIndex)
{
    // 선택된 아이템 정보 표시
    if (slotIndex >= 0 && slotIndex < inventoryItems.Num())
    {
        const FInventoryItemData& item = inventoryItems[slotIndex];
        
        // 아이템 이름 표시
        if (hoveredItemNameText != nullptr)
            hoveredItemNameText->SetText(FText::FromString(item.itemName));
        
        // 아이템 상세 정보 표시
        if (hoveredItemMoreInfo != nullptr)
        {
            FString infoText = FString::Printf(TEXT("[Inventory] %s\n수량: %d\n%s"), *item.itemType, item.count, *item.itemDescription);
            hoveredItemMoreInfo->SetText(FText::FromString(infoText));
        }
    }

    else
    {
        // 선택된 아이템이 없을 때 정보창 초기화
        if (hoveredItemNameText != nullptr)
            hoveredItemNameText->SetText(FText::FromString(TEXT("")));
        
        if (hoveredItemMoreInfo != nullptr)
            hoveredItemMoreInfo->SetText(FText::FromString(TEXT("")));
    }
}

void UInventory::UpdateHealthStatus()
{
    // 기본 체력 상태 표시
    if (healthStatusText != nullptr)
        healthStatusText->SetText(FText::FromString(TEXT("FINE")));
}

int32 UInventory::CalculateSlotIndex(UWidget* SlotWidget)
{
    // 슬롯 위젯에서 인덱스 추출
    UInventorySlot* clickedSlot = Cast<UInventorySlot>(SlotWidget);
    if (clickedSlot)
    {
        return clickedSlot->GetSlotIndex();
    }
    
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 클릭된 위젯이 InventorySlot이 아닙니다."));
    return -1;
}

void UInventory::OnTopShortcutClicked()
{
    // 상단 단축키 처리
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 상단 단축키 클릭됨"));
}

void UInventory::OnRightShortcutClicked()
{
    // 우측 단축키 처리
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 우측 단축키 클릭됨"));
}

void UInventory::OnBotShortcutClicked()
{
    // 하단 단축키 처리
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 하단 단축키 클릭됨"));
}

void UInventory::OnLeftShortcutClicked()
{
    // 좌측 단축키 처리
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[Inventory] 좌측 단축키 클릭됨"));
}

bool UInventory::AddItem(const FInventoryItemData& newItem)
{
    // 인벤토리 여유 공간 확인 후 아이템 추가
    if (inventoryItems.Num() < 20)
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
    // 슬롯 인덱스 유효성 검사
    if (slotIndex < 0 || slotIndex >= inventoryItems.Num())
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 유효하지 않은 슬롯 인덱스"));
        return false;
    }

    FString itemName = inventoryItems[slotIndex].itemName;
    inventoryItems.RemoveAt(slotIndex);
    UpdateInventoryDisplay();

    // 선택된 슬롯이 제거된 경우 선택 해제
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
    // 선택된 슬롯과 사용 슬롯 일치 확인
    if (currentSelectedSlotIndex != slotIndex)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 현재 선택된 슬롯과 사용하려는 슬롯이 다릅니다."));
        return false;
    }

    FInventoryItemData& item = inventoryItems[slotIndex];

    // 아이템 사용 처리
    if (item.count > 0)
    {
        item.count--;

        // 수량이 0이 되면 아이템 제거
        if (item.count <= 0)
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
    // 인벤토리 UI 표시
    SetVisibility(ESlateVisibility::Visible);
    UpdateInventoryDisplay();
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("[Inventory] 인벤토리 표시됨"));
}

void UInventory::HideInventory()
{
    // 인벤토리 UI 숨김
    SetVisibility(ESlateVisibility::Hidden);
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("[Inventory] 인벤토리 숨겨짐"));
}

FInventoryItemData UInventory::GetCurrentSelectedItem() const
{
    // 현재 선택된 아이템 반환
    if (currentSelectedSlotIndex >= 0 && currentSelectedSlotIndex < inventoryItems.Num())
        return inventoryItems[currentSelectedSlotIndex];

    return FInventoryItemData(); 
}

void UInventory::SetHealthStatus(const FString& healthText)
{
    // 체력 상태 텍스트 설정
    if (healthStatusText)
        healthStatusText->SetText(FText::FromString(healthText));
}