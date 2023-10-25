// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Horizontal.h"

namespace EWN::WidgetNavigation
{
bool FCursorHandler_Horizontal::IsHorizontal( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UHorizontalBox>();
}

int32 FCursorHandler_Horizontal::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const
{
	int32 ResultIndex = CurrentIndex;

	switch ( WidgetCursor )
	{
	case EEWN_WidgetCursor::Left:
	{
		ResultIndex = GetBackwardIndex( CurrentIndex, bLoopIgnored );
	}
	break;

	case EEWN_WidgetCursor::Right:
	{
		ResultIndex = GetForwardIndex( CurrentIndex, bLoopIgnored );
	}
	break;

	default:;
	}

	return ResultIndex;
}
}	 // namespace EWN::WidgetNavigation
