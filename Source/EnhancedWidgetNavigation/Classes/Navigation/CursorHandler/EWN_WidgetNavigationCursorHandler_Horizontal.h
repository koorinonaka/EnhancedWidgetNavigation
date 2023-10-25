// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/HorizontalBox.h"
#include "EWN_WidgetNavigationCursorHandler.h"

namespace EWN::WidgetNavigation
{
class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Horizontal final : public FCursorHandler
{
public:
	static bool IsHorizontal( UPanelWidget* PanelWidget );

public:
	explicit FCursorHandler_Horizontal( UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation ) {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const override;
};
}	 // namespace EWN::WidgetNavigation
