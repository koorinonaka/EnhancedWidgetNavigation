// Copyright 2020- koorinonaka, Inc. All Rights Reserved.

#include "Modules/ModuleManager.h"

class FEnhancedWidgetNavigationModule : public IModuleInterface
{
public:
	virtual bool IsGameModule() const override { return true; }
};

IMPLEMENT_MODULE( FEnhancedWidgetNavigationModule, EnhancedWidgetNavigation )
