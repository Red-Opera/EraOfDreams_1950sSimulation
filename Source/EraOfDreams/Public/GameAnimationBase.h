// Copyright (c) 2024 Marvin Bernd Watanabe

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FGameAnimationBaseModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
