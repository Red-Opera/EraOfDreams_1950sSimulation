// Copyright (c) 2024 Marvin Bernd Watanabe

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Interface.h"
#include "InteractionTransformInterface.generated.h"

UINTERFACE()
class UInteractionTransformInterface : public UInterface
{
	GENERATED_BODY()
};

class ERAOFDREAMS_API IInteractionTransformInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
		void SetInteractionTransform(const FTransform& InteractionTransform);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Traversal")
		void GetInteractionTransform(FTransform& OutInteractionTransform) const;
};

UCLASS(meta = (BlueprintThreadSafe))
class ERAOFDREAMS_API UInteractionTransformBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Traversal")
		static UPARAM(DisplayName = "Success") bool SetInteractionTransform(UObject* InteractionTransformObject, const FTransform& InteractionTransform);
	UFUNCTION(BlueprintCallable, Category="Traversal")
		static UPARAM(DisplayName = "Success") bool GetInteractionTransform(UObject* InteractionTransformObject, UPARAM(DisplayName = "InteractionTransform") FTransform& OutInteractionTransform);
};
