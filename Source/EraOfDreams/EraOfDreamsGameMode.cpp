// Copyright Epic Games, Inc. All Rights Reserved.

#include "EraOfDreamsGameMode.h"
#include "EraOfDreamsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEraOfDreamsGameMode::AEraOfDreamsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
