// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

class ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetNavigationCursorHandler_Horizontal : public FEWN_WidgetNavigationCursorHandler
{
public:
	static bool IsHorizontal( class UPanelWidget* PanelWidget );

public:
	FEWN_WidgetNavigationCursorHandler_Horizontal( class UEWN_WidgetNavigation* Navigation );

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override;
};
