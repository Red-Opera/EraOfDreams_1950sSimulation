#include "UI/PlayerMenu.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "KeyInputManager.h"

void UPlayerMenu::NativeConstruct()
{
    Super::NativeConstruct();

    // 인벤토리 초기 상태 설정
    isInventoryOpen = false;

    // 위젯 계층에서 인벤토리 위젯 찾기 시도
    inventory = Cast<UInventory>(GetWidgetFromName(TEXT("inventoryBlueprint")));

    // 인벤토리 위젯을 찾을 수 없을 때 오류 메시지 출력
    if (inventory == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[PlayerMenu] inventory가 nullptr입니다. PlayerMenu 위젯이 올바르게 설정되었는지 확인하세요."));

        return;
    }

    // 테스트용 아이템 추가
    AddTestItems();

    // 디버그 메시지
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("[PlayerMenu] NativeConstruct 완료"));
}

void UPlayerMenu::NativeDestruct()
{
    Super::NativeDestruct();
}

FReply UPlayerMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& inKeyEvent)
{
    // ESC 키로 인벤토리 닫기
    if (inKeyEvent.GetKey() == EKeys::Escape)
    {
        HandleEscapeKey();

        return FReply::Handled();
    }
    
    return Super::NativeOnKeyDown(InGeometry, inKeyEvent);
}

void UPlayerMenu::RegisterInputEvents()
{
    // 디버그 메시지
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("[PlayerMenu] 입력 이벤트 등록 생략됨 (Sera에서 처리)"));
}

void UPlayerMenu::UnregisterInputEvents()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("[PlayerMenu] 입력 이벤트 해제 완료"));
}

void UPlayerMenu::HandleInventoryToggle()
{
    // 인벤토리 위젯이 유효한지 확인
    if (inventory == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[PlayerMenu] 인벤토리 위젯이 없어 토글할 수 없습니다."));

        return;
    }
    
    // 인벤토리 토글 실행
    ToggleInventory();
    
    // 디버그 메시지
    FString statusMessage = isInventoryOpen ? TEXT("[PlayerMenu] 인벤토리 열림") : TEXT("[PlayerMenu] 인벤토리 닫힘");
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, statusMessage);
}

void UPlayerMenu::HandleEscapeKey()
{
    // ESC 키는 인벤토리가 열려있을 때만 닫기
    if (!isInventoryOpen)
        return;

    HideInventory();

    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("[PlayerMenu] ESC로 인벤토리 닫음"));
}

void UPlayerMenu::ToggleInventory()
{
    if (isInventoryOpen)
        HideInventory();

    else
        ShowInventory();
}

void UPlayerMenu::ShowInventory()
{
    // 인벤토리 위젯 유효성 확인
    if (inventory == nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[PlayerMenu] 인벤토리 위젯이 없어 표시할 수 없습니다."));

        return;
    }
    
    // 인벤토리 표시
    inventory->ShowInventory();
    isInventoryOpen = true;

	APlayerController* playerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;

    // 인벤토리가 열렸을 때 GameAndUI 입력 모드로 변경 (UI만 사용하는 대신)
    if (GetWorld() && playerController != nullptr)
    {
        FInputModeGameAndUI inputMode;

        inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);    // 마우스가 뷰포트에 갇히지 않도록 설정
        inputMode.SetHideCursorDuringCapture(false);                            // 캡처 중 커서 숨김 비활성화
        
        playerController->SetInputMode(inputMode);
        playerController->bShowMouseCursor = true;
    }
    
    // 디버그 메시지
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("[PlayerMenu] 인벤토리 표시됨 - UI 모드로 전환"));
}

void UPlayerMenu::HideInventory()
{
    // 인벤토리 위젯 유효성 확인
    if (inventory == nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("[PlayerMenu] 인벤토리 위젯이 없어 숨길 수 없습니다."));

        return;
    }
    
    // 인벤토리 숨김
    inventory->HideInventory();
    isInventoryOpen = false;

	// 플레이어 컨트롤러가 유효한지 확인
	APlayerController* playerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;

    // 인벤토리가 닫혔을 때 게임 전용 입력 모드로 복원
    if (GetWorld() && playerController != nullptr)
    {
        playerController->SetInputMode(FInputModeGameOnly());
        playerController->bShowMouseCursor = false;
    }
    
    // 디버그 메시지
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("[PlayerMenu] 인벤토리 숨김 - 게임 전용 모드로 복원"));
}

void UPlayerMenu::AddTestItems()
{
    if (inventory == nullptr)
    {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("[PlayerMenu] inventory가 nullptr입니다. PlayerMenu 위젯이 올바르게 설정되었는지 확인하세요."));

        return;
    }

    // 테스트용 아이템들 추가
    FInventoryItemData testItem1;
    testItem1.itemName = TEXT("M1897");
    testItem1.itemType = TEXT("무기");
    testItem1.quantity = 12;
    testItem1.itemDescription = TEXT("강력한 샷건");

    inventory->AddItem(testItem1);

    FInventoryItemData testItem2;
    testItem2.itemName = TEXT("응급치료키트");
    testItem2.itemType = TEXT("의료용품");
    testItem2.quantity = 3;
    testItem2.itemDescription = TEXT("체력을 회복시켜줍니다");
    inventory->AddItem(testItem2);

    FInventoryItemData testItem3;
    testItem3.itemName = TEXT("열쇠");
    testItem3.itemType = TEXT("도구");
    testItem3.quantity = 1;
    testItem3.itemDescription = TEXT("특정 문을 열 수 있습니다");
    inventory->AddItem(testItem3);

	// 디버그 메시지
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("[PlayerMenu] 테스트 아이템 추가 완료"));
}