// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Vertical.h"

namespace EWN::WidgetNavigation
{
bool FCursorHandler_Vertical::IsVertical( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UVerticalBox>();
}

int32 FCursorHandler_Vertical::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
{
	int32 ResultIndex = CurrentIndex;

	switch ( WidgetCursor )
	{
	case EEWN_WidgetCursor::Up:
	{
		ResultIndex = GetBackwardIndex( CurrentIndex );
	}
	break;

	case EEWN_WidgetCursor::Down:
	{
		ResultIndex = GetForwardIndex( CurrentIndex );
	}
	break;

	default:;
	}

	return ResultIndex;
}
}	 // namespace EWN::WidgetNavigation
