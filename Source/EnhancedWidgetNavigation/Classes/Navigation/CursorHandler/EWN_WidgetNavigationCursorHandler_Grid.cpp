// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"

namespace EWN::WidgetNavigation
{
bool FCursorHandler_Grid::IsGrid( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UGridPanel>() || PanelWidget->IsA<UUniformGridPanel>();
}

int32 FCursorHandler_Grid::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
{
	return IsDistanceBasedNavigation() ? GetNextIndexFromDistanceBased( CurrentIndex, WidgetCursor )
									   : GetNextIndexFromSimple( CurrentIndex, WidgetCursor );
}
}	 // namespace EWN::WidgetNavigation
