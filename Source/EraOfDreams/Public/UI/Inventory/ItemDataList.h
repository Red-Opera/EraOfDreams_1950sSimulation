#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Inventory/ItemData.h"
#include "ItemDataList.generated.h"

// 게임 내 여러 아이템 정보를 에디터에서 관리할 수 있는 데이터 에셋
UCLASS(BlueprintType)
class ERAOFDREAMS_API UItemDataList : public UDataAsset
{
    GENERATED_BODY()

public:
    // 이 데이터 에셋에 정의된 모든 아이템 목록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (DisplayName = "아이템 목록", TitleProperty = "itemName"))
    TArray<FItemData> itemList;

    // 아이템 ID로 아이템 데이터 조회
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FItemData GetItemByName(const FString& itemName) const;

    // 모든 아이템 목록 반환
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FItemData> GetAllItems() const;

    // 특정 타입의 아이템만 필터링하여 반환
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FItemData> GetItemsByType(const FString& itemType) const;

    // 아이템 카테고리 목록 반환 (중복 제거된 유니크 목록)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FString> GetItemCategories() const;
};
