// Copyright (c) 2024 Marvin Bernd Watanabe

#include "Samples/GameAnimationSampleGameMode.h"

#include "Samples/GameAnimationSampleCharacter.h"
#include "Samples/GameAnimationSampleGameState.h"
#include "Samples/GameAnimationSamplePlayerController.h"
#include "Samples/GameAnimationSamplePlayerState.h"

AGameAnimationSampleGameMode::AGameAnimationSampleGameMode()
{
	DefaultPawnClass = AGameAnimationSampleCharacter::StaticClass();
	GameStateClass = AGameAnimationSampleGameState::StaticClass();
	PlayerStateClass = AGameAnimationSamplePlayerState::StaticClass();
	PlayerControllerClass = AGameAnimationSamplePlayerController::StaticClass();
}
