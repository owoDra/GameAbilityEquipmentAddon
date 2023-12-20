// Copyright (C) 2023 owoDra

#pragma once

#include "Modules/ModuleManager.h"

/**
 *  Modules for the main features of the Game Ability: Equipment Addon plugin
 */
class FGAEAddonModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
