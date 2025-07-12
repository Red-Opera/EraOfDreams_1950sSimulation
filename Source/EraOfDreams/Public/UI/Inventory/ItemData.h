#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

// 게임 내 아이템 데이터를 정의하는 구조체
USTRUCT(BlueprintType)
struct FItemData
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
    FItemData()
    {
        itemIcon = nullptr;
        itemName = TEXT("");
        count = 0;
        itemType = TEXT("");
        itemDescription = TEXT("");
    }
};