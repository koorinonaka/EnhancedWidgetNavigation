// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Horizontal.h"

namespace EWN::WidgetNavigation
{
bool FCursorHandler_Horizontal::IsHorizontal( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UHorizontalBox>();
}

int32 FCursorHandler_Horizontal::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
{
	int32 ResultIndex = CurrentIndex;

	switch ( WidgetCursor )
	{
	case EEWN_WidgetCursor::Left:
	{
		ResultIndex = GetBackwardIndex( CurrentIndex );
	}
	break;

	case EEWN_WidgetCursor::Right:
	{
		ResultIndex = GetForwardIndex( CurrentIndex );
	}
	break;
	}

	return ResultIndex;
}
}	 // namespace EWN::WidgetNavigation
