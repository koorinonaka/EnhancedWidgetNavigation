// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

//
#include "Components/HorizontalBox.h"

namespace EWN::WidgetNavigation
{
class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Horizontal : public FCursorHandler
{
public:
	static bool IsHorizontal( UPanelWidget* PanelWidget );

public:
	FCursorHandler_Horizontal( UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation ) {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override;
};
}	 // namespace EWN::WidgetNavigation
