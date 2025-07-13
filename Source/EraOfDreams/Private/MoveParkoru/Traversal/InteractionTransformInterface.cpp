// Copyright (c) 2024 Marvin Bernd Watanabe

#include "MoveParkoru/Traversal/InteractionTransformInterface.h"

bool UInteractionTransformBlueprintFunctionLibrary::SetInteractionTransform(UObject* InteractionTransformObject,
	const FTransform& InteractionTransform)
{
	if (IsValid(InteractionTransformObject) &&
		InteractionTransformObject->Implements<UInteractionTransformInterface>())
	{
		IInteractionTransformInterface::Execute_SetInteractionTransform(InteractionTransformObject, InteractionTransform);
		return true;
	}
	return false;
}

bool UInteractionTransformBlueprintFunctionLibrary::GetInteractionTransform(UObject* InteractionTransformObject,
	FTransform& OutInteractionTransform)
{
	if (IsValid(InteractionTransformObject) &&
		InteractionTransformObject->Implements<UInteractionTransformInterface>())
	{
		IInteractionTransformInterface::Execute_GetInteractionTransform(InteractionTransformObject, OutInteractionTransform);
		return true;
	}
	return false;
}
