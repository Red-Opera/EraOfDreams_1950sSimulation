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
#include "UI/Inventory/ItemDataList.h"
#include "UI/Inventory/ItemData.h"

UInventory::UInventory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본 값 초기화
    currentSelectedSlotIndex = -1;
    inventorySlotClass = UInventorySlot::StaticClass();
    itemDataList = nullptr;
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

void UInventory::SetItemDataList(UItemDataList* dataList)
{
    itemDataList = dataList;

    if (itemDataList != nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
            FString::Printf(TEXT("[Inventory] 아이템 데이터 리스트 설정됨: %d개 아이템"), dataList->GetAllItems().Num()));
    }

    else
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 경고: 유효하지 않은 아이템 데이터 리스트"));
}

bool UInventory::AddItemByName(const FString& itemName, int32 count)
{
    // 데이터 에셋 유효성 검사
    if (itemDataList == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
            TEXT("[Inventory] 오류: 아이템 데이터 리스트가 설정되지 않았습니다"));

        return false;
    }
    
    // 아이템 데이터 조회
    FItemData itemData = itemDataList->GetItemByName(itemName);
    
    if (itemData.itemName.IsEmpty())
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
            FString::Printf(TEXT("[Inventory] 오류: 아이템 이름 '%s'을(를) 찾을 수 없습니다"), *itemName));
        return false;
    }
    
    // 수량 설정
    itemData.count = count;
    
    // 아이템 추가
    return AddItem(itemData);
}

UInventorySlot* UInventory::CreateInventorySlot(int32 slotIndex)
{
    // 슬롯 클래스 유효성 검사
    if (inventorySlotClass == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, 
            TEXT("[Inventory] 경고: 인벤토리 슬롯 클래스가 설정되지 않았습니다. 블루프린트에서 인벤토리 슬롯 클래스를 설정하세요."));

        return nullptr;
    }
    
    // 슬롯 위젯 생성 및 설정
    UInventorySlot* newSlot = CreateWidget<UInventorySlot>(GetOwningPlayer(), inventorySlotClass);
    
    if (newSlot != nullptr)
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
    if (inventorySlotClass == nullptr)
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

                if (vBoxSlot != nullptr)
                    vBoxSlot->SetPadding(FMargin(0, 0, 0, slotVerticalSpacing));
            }
        }
    }

    // 각 행에 슬롯 생성 (5행 x 4열 = 20개)
    for (int32 boxIndex = 0; boxIndex < slotHorizontalBoxes.Num(); boxIndex++)
    {
        UHorizontalBox* currentHBox = slotHorizontalBoxes[boxIndex];

        if (currentHBox == nullptr)
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
                    hBoxSlot->SetPadding(FMargin(0, 0, slotHorizontalSpacing, 0));
                
                inventorySlots.Add(newSlot);
                newSlot->SetItemCount(0);
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
        if (inventorySlots.IsValidIndex(i) && inventorySlots[i] != nullptr)
        {
            inventorySlots[i]->SetItemIcon(nullptr);
            inventorySlots[i]->SetItemCount(0);
        }
    }

    if (itemDataList == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] itemDataList가 유효하지 않습니다."));

        return;
    }

    // 아이템 데이터로 슬롯 UI 갱신
    for (int32 i = 0; i < itemDataList->itemList.Num() && i < inventorySlots.Num(); i++)
    {
        if (inventorySlots.IsValidIndex(i) && inventorySlots[i] != nullptr)
        {
            const FItemData& itemData = itemDataList->itemList[i];
            inventorySlots[i]->SetItemIcon(itemData.itemIcon);
            inventorySlots[i]->SetItemCount(itemData.count);
        }
    }
}

void UInventory::UpdateSelectedItemInfo(int32 slotIndex)
{
    if (itemDataList == nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] itemDataList가 유효하지 않습니다."));

        return;
    }

    // 선택된 아이템 정보 표시
    if (slotIndex >= 0 && slotIndex < itemDataList->itemList.Num())
    {
        const FItemData& itemData = itemDataList->itemList[slotIndex];

        if (hoveredItemNameText != nullptr)
            hoveredItemNameText->SetText(FText::FromString(itemData.itemName));

        if (hoveredItemMoreInfo != nullptr)
            hoveredItemMoreInfo->SetText(FText::FromString(itemData.itemDescription));
    }

    else
    {
        // 선택 해제 시 정보 초기화
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

    if (clickedSlot != nullptr)
        return clickedSlot->GetSlotIndex();
    
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 클릭된 위젯이 InventorySlot이 아닙니다."));

    return -1;
}

bool UInventory::AddItem(const FItemData& newItem)
{
    if (itemDataList == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] itemDataList가 유효하지 않습니다."));

        return false;
    }

    // 인벤토리 여유 공간 확인 후 아이템 추가
    if (itemDataList->itemList.Num() < 20)
    {
        itemDataList->itemList.Add(newItem);
        UpdateInventoryDisplay();

        return true;
    }
    
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] 인벤토리가 가득참"));

    return false;
}

bool UInventory::RemoveItem(int32 slotIndex)
{
    if (itemDataList == nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] itemDataList가 유효하지 않습니다."));

        return false;
    }

    // 슬롯 인덱스 유효성 검사
    if (itemDataList->itemList.IsValidIndex(slotIndex))
    {
        itemDataList->itemList.RemoveAt(slotIndex);
        UpdateInventoryDisplay();

        return true;
    }

    return false;
}

bool UInventory::UseItem(int32 slotIndex)
{
    if (itemDataList == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] itemDataList가 유효하지 않습니다."));

        return false;
    }

    if (itemDataList->itemList.IsValidIndex(slotIndex))
    {
        // 아이템 사용 로직 (수량 감소)
        itemDataList->itemList[slotIndex].count--;

        if (itemDataList->itemList[slotIndex].count <= 0)
            RemoveItem(slotIndex);

        else
            UpdateInventoryDisplay();

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
    SetVisibility(ESlateVisibility::Hidden);
}

FItemData UInventory::GetCurrentSelectedItem() const
{
    if (itemDataList == nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[Inventory] itemDataList가 유효하지 않습니다."));

        return FItemData();
    }

    if (currentSelectedSlotIndex >= 0 && currentSelectedSlotIndex < itemDataList->itemList.Num())
        return itemDataList->itemList[currentSelectedSlotIndex];

    return FItemData();
}

void UInventory::SetHealthStatus(const FString& healthText)
{
    // 체력 상태 텍스트 설정
    if (healthStatusText)
        healthStatusText->SetText(FText::FromString(healthText));
}