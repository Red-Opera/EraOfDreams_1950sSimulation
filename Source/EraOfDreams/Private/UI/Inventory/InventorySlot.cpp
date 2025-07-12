#include "UI/Inventory/InventorySlot.h"
#include "Engine/Engine.h"

UInventorySlot::UInventorySlot(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본값 초기화
    slotIndex = -1;
    isSelected = false;
    isHighlighted = false;
    isEquipped = false;
    isDragging = false;
}

void UInventorySlot::NativeConstruct()
{
    Super::NativeConstruct();
    
    // 초기 상태 설정 - 이미지 알파 값 조정
    if (selectedImage != nullptr) 
    {
        selectedImage->SetVisibility(ESlateVisibility::Visible);
        selectedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
    }
    
    if (hoverImage != nullptr) 
    {
        hoverImage->SetVisibility(ESlateVisibility::Visible);
        hoverImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
    }
    
    if (highlightImage != nullptr) 
    {
        highlightImage->SetVisibility(ESlateVisibility::Visible);
        highlightImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
    }
    
    if (equippedText != nullptr) 
    {
        equippedText->SetText(FText::FromString(TEXT("E")));
        equippedText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventorySlot::NativeDestruct()
{
    Super::NativeDestruct();
}

FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 왼쪽 마우스 버튼 클릭 확인
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 슬롯 클릭 이벤트 발생
        OnSlotClicked.Broadcast(slotIndex);
        
        // 이벤트가 처리되었음을 알림
        return FReply::Handled();
    }
    
    // 다른 버튼 클릭은 부모 클래스로 위임
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    
    // 마우스 호버 상태 표시 - 알파 값 조정
    if (hoverImage)
    {
        hoverImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    }
    
    // 선택 및 하이라이트 상태 유지 (isSelected, isHighlighted 값에 따라)
    UpdateVisualState();
}

void UInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    
    // 마우스 호버 상태 해제 - 알파 값 조정
    if (hoverImage)
    {
        hoverImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
    }
    
    // 선택 및 하이라이트 상태 유지
    UpdateVisualState();
}

// 시각적 상태 업데이트 헬퍼 함수
void UInventorySlot::UpdateVisualState()
{
    // 선택 상태 적용
    if (selectedImage != nullptr)
        selectedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, isSelected ? 1.0f : 0.0f));
    
    // 하이라이트 상태 적용
    if (highlightImage != nullptr)
        highlightImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, isHighlighted ? 1.0f : 0.0f));
}

void UInventorySlot::SetSlotIndex(int32 index)
{
    slotIndex = index;
}

void UInventorySlot::SetItemIcon(UTexture2D* icon)
{
    if (itemImage == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("[InventorySlot] itemImage가 설정되지 않았습니다."));

        return;
    }

    if (icon)
    {
        itemImage->SetBrushFromTexture(icon);
        itemImage->SetVisibility(ESlateVisibility::Visible);
    }

    else
    {
        itemImage->SetBrushFromTexture(nullptr);
        itemImage->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventorySlot::SetItemCount(int32 count)
{
    if (countText == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("[InventorySlot] countText가 설정되지 않았습니다."));

        return;
    }

    if (count > 1)
    {
        countText->SetText(FText::FromString(FString::FromInt(count)));
        countText->SetVisibility(ESlateVisibility::Visible);
    }

    else
    {
        countText->SetText(FText::FromString(TEXT("")));
        countText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventorySlot::SetSelected(bool isNewSelected)
{
    this->isSelected = isNewSelected;
    
    // 알파 값 설정으로 변경
    if (selectedImage != nullptr)
        selectedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, isNewSelected ? 1.0f : 0.0f));
    
    // 다른 상태 업데이트
    UpdateVisualState();
}

void UInventorySlot::SetHighlighted(bool isNewHighlight)
{
    this->isHighlighted = isNewHighlight;
    
    // 알파 값 설정으로 변경
    if (highlightImage != nullptr)
        highlightImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, isNewHighlight ? 1.0f : 0.0f));
    
    // 다른 상태 업데이트
    UpdateVisualState();
}

void UInventorySlot::SetEquipped(bool isNewEquipped)
{
    this->isEquipped = isNewEquipped;
    
    if (equippedText != nullptr)
        equippedText->SetVisibility(isNewEquipped ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
