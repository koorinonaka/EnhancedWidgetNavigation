// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationCursorHandler_Horizontal.h"

//
#include "Components/HorizontalBox.h"

bool FEWN_WidgetNavigationCursorHandler_Horizontal::IsHorizontal( UPanelWidget* PanelWidget )
{
	return PanelWidget->IsA<UHorizontalBox>();
}

FEWN_WidgetNavigationCursorHandler_Horizontal::FEWN_WidgetNavigationCursorHandler_Horizontal( UEWN_WidgetNavigation* Navigation )
	: FEWN_WidgetNavigationCursorHandler( Navigation )
{
}

int32 FEWN_WidgetNavigationCursorHandler_Horizontal::GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
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
