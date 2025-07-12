#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Engine/Texture2D.h"
#include "InventorySlot.generated.h"

/**
 * 개별 인벤토리 슬롯을 표현하는 위젯 클래스
 */
UCLASS()
class ERAOFDREAMS_API UInventorySlot : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UInventorySlot(const FObjectInitializer& ObjectInitializer);
    
    // 슬롯 인덱스 설정
    UFUNCTION(BlueprintCallable)
    void SetSlotIndex(int32 Index);
    
    // 슬롯 인덱스 반환
    UFUNCTION(BlueprintPure)
    int32 GetSlotIndex() const { return slotIndex; }
    
    // 아이템 아이콘 설정
    UFUNCTION(BlueprintCallable)
    void SetItemIcon(UTexture2D* icon);
    
    // 아이템 수량 설정
    UFUNCTION(BlueprintCallable)
    void SetItemCount(int32 count);
    
    // 슬롯 선택 상태 설정
    UFUNCTION(BlueprintCallable)
    void SetSelected(bool isNewSelected);
    
    // 슬롯 하이라이트 상태 설정
    UFUNCTION(BlueprintCallable)
    void SetHighlighted(bool isNewHighlight);
    
    // 장착 상태 설정
    UFUNCTION(BlueprintCallable)
    void SetEquipped(bool isNewEquipped);
    
    // 슬롯 클릭 이벤트 위임
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClickedDelegate, int32, SlotIndex);
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnSlotClickedDelegate OnSlotClicked;
    
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    
    // 마우스 이벤트 - 이제 여기서 직접 클릭을 처리합니다
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    // 시각적 상태 업데이트 (선택, 하이라이트 등)
    void UpdateVisualState();

private:
    // 슬롯의 인덱스
    UPROPERTY()
    int32 slotIndex;
    
    // 슬롯 상태 변수들
    UPROPERTY()
    bool isSelected;
    
    UPROPERTY()
    bool isHighlighted;
    
    UPROPERTY()
    bool isEquipped;
    
    UPROPERTY()
    bool isDragging;
    
    // UI 요소들
public:
    // 메인 오버레이 (모든 요소를 포함)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UOverlay* mainOverlay;
    
    // 배경 이미지
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UImage* backgroundImage;
    
    // 아이템 아이콘 이미지
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UImage* itemImage;
    
    // 선택 상태 이미지
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UImage* selectedImage;
    
    // 마우스 호버 이미지
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UImage* hoverImage;
    
    // 하이라이트 이미지
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UImage* highlightImage;
    
    // 수량 표시 텍스트
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* countText;
    
    // 장착 표시 텍스트 (E)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* equippedText;
};
