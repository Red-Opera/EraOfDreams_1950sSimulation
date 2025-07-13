// Copyright (c) 2024 Marvin Bernd Watanabe

#include "MoveParkoru/Core/GameAnimationBaseGameInstanceSubsystem.h"

void UGameAnimationBaseGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.DrawMeshTrajectory", DrawMeshTrajectory);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.MMDrawQuery", MMDrawQuery);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.MMDrawMatch", MMDrawMatch);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.DrawCharacterDebugShapes", DrawCharacterDebugShapes);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.MMUseStateMachine", MMUseStateMachine);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.MMUseSets", MMUseSets);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.Traversal.DrawDebugLevel", DrawDebugLevel);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.Traversal.DrawDebugDuration", DrawDebugDuration);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.MMDatabaseLOD", MMDatabaseLOD);
	REGISTER_CONSOLE_VARIABLE_CHANGE("DDCvar.DrawVisLogShapesForFoleySounds", DrawVisLogShapesForFoleySounds);
	REGISTER_CONSOLE_VARIABLE_CHANGE("a.animnode.offsetrootbone.enable", OffsetRootBoneEnable);
	
	Super::Initialize(Collection);
}
