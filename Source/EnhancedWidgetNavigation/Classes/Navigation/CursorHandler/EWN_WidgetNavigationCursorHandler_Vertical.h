// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

//
#include "Components/VerticalBox.h"

namespace EWN::WidgetNavigation
{
class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Vertical : public FCursorHandler
{
public:
	static bool IsVertical( UPanelWidget* PanelWidget );

public:
	FCursorHandler_Vertical( UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation ) {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override;
};
}	 // namespace EWN::WidgetNavigation
