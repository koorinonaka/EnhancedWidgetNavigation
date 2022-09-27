// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"

bool FEWN_WidgetNavigationCursorHandler_Grid::IsGrid( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UGridPanel>() || PanelWidget->IsA<UUniformGridPanel>();
}

FEWN_WidgetNavigationCursorHandler_Grid::FEWN_WidgetNavigationCursorHandler_Grid( UEWN_WidgetNavigation* Navigation )
	: FEWN_WidgetNavigationCursorHandler( Navigation )
{
	InitSimple();
	InitDistanceBased();
}

int32 FEWN_WidgetNavigationCursorHandler_Grid::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
{
	return IsDistanceBasedNavigation() ? GetNextIndexFromDistanceBased( CurrentIndex, WidgetCursor )
									   : GetNextIndexFromSimple( CurrentIndex, WidgetCursor );
}
