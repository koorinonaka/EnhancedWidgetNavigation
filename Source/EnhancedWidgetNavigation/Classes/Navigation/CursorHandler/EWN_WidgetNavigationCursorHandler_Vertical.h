// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/VerticalBox.h"
#include "EWN_WidgetNavigationCursorHandler.h"

namespace EWN::WidgetNavigation
{
class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Vertical final : public FCursorHandler
{
public:
	static bool IsVertical( UPanelWidget* PanelWidget );

public:
	explicit FCursorHandler_Vertical( UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation ) {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override;
};
}	 // namespace EWN::WidgetNavigation
