#include "UI/Inventory/ItemDataList.h"

FItemData UItemDataList::GetItemByName(const FString& itemName) const
{
    // 이름으로 아이템 찾기
    for (const FItemData& item : itemList)
    {
        if (item.itemName.Equals(itemName, ESearchCase::IgnoreCase))
            return item;
    }

    // 아이템을 찾지 못한 경우 빈 아이템 데이터 반환
    return FItemData();
}

TArray<FItemData> UItemDataList::GetAllItems() const
{
    return itemList;
}

TArray<FItemData> UItemDataList::GetItemsByType(const FString& itemType) const
{
    TArray<FItemData> filteredItems;

    // 지정된 타입과 일치하는 모든 아이템 필터링
    for (const FItemData& item : itemList)
    {
        if (item.itemType.Equals(itemType, ESearchCase::IgnoreCase))
            filteredItems.Add(item);
    }

    return filteredItems;
}

TArray<FString> UItemDataList::GetItemCategories() const
{
    TArray<FString> categories;

    // 모든 아이템을 순회하며 카테고리 추출
    for (const FItemData& item : itemList)
    {
        if (!categories.Contains(item.itemType))
            categories.Add(item.itemType);
    }

    return categories;
}