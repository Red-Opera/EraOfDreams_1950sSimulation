// Copyright (c) 2024 Marvin Bernd Watanabe

#include "MoveParkoru/Samples/GameAnimationSampleGameMode.h"

#include "MoveParkoru/Samples/GameAnimationSampleCharacter.h"
#include "MoveParkoru/Samples/GameAnimationSampleGameState.h"
#include "MoveParkoru/Samples/GameAnimationSamplePlayerController.h"
#include "MoveParkoru/Samples/GameAnimationSamplePlayerState.h"

AGameAnimationSampleGameMode::AGameAnimationSampleGameMode()
{
	DefaultPawnClass = AGameAnimationSampleCharacter::StaticClass();
	GameStateClass = AGameAnimationSampleGameState::StaticClass();
	PlayerStateClass = AGameAnimationSamplePlayerState::StaticClass();
	PlayerControllerClass = AGameAnimationSamplePlayerController::StaticClass();
}
