// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationCursorHandler_Vertical.h"

//
#include "Components/VerticalBox.h"

bool FEWN_WidgetNavigationCursorHandler_Vertical::IsVertical( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UVerticalBox>();
}

FEWN_WidgetNavigationCursorHandler_Vertical::FEWN_WidgetNavigationCursorHandler_Vertical( UEWN_WidgetNavigation* Navigation )
	: FEWN_WidgetNavigationCursorHandler( Navigation )
{
}

int32 FEWN_WidgetNavigationCursorHandler_Vertical::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
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
	}

	return ResultIndex;
}
