// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"

// Engine
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

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
