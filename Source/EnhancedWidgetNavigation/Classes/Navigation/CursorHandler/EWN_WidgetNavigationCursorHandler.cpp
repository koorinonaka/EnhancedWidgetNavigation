// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationCursorHandler.h"

//
#include "Components/PanelWidget.h"

//
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"
#include "Navigation/EWN_WidgetNavigation.h"

//
#include "EWN_WidgetNavigationCursorHandler_Grid.h"
#include "EWN_WidgetNavigationCursorHandler_Horizontal.h"
#include "EWN_WidgetNavigationCursorHandler_Vertical.h"

UEWN_WidgetNavigationCursorHandler* FEWN_WidgetNavigationCursorFactory::CreateHandler( UEWN_WidgetNavigation* Navigation )
{
	auto* PanelWidget = Navigation->GetTypedOuter<UPanelWidget>();
	if ( PanelWidget && !PanelWidget->IsDesignTime() )
	{
		if ( UEWN_WidgetNavigationCursorHandler_Vertical::IsVertical( PanelWidget ) )
		{
			return NewObject<UEWN_WidgetNavigationCursorHandler_Vertical>( Navigation );
		}
		else if ( UEWN_WidgetNavigationCursorHandler_Horizontal::IsHorizontal( PanelWidget ) )
		{
			return NewObject<UEWN_WidgetNavigationCursorHandler_Horizontal>( Navigation );
		}
		else if ( UEWN_WidgetNavigationCursorHandler_Grid::IsGrid( PanelWidget ) )
		{
			return NewObject<UEWN_WidgetNavigationCursorHandler_Grid>( Navigation );
		}
		else
		{
			// fallback for PanelWidget that is not navigative like SizeBox.
			return NewObject<UEWN_WidgetNavigationCursorHandler_Default>( Navigation );
		}
	}

	return nullptr;
}

UPanelWidget* UEWN_WidgetNavigationCursorHandler::GetPanelWidget() const
{
	return GetTypedOuter<UPanelWidget>();
}

bool UEWN_WidgetNavigationCursorHandler::IsLoopNavigation() const
{
	return GetTypedOuter<UEWN_WidgetNavigation>()->bLoopNavigation;
}

bool UEWN_WidgetNavigationCursorHandler::IsWrapLines() const
{
	return GetTypedOuter<UEWN_WidgetNavigation>()->bWrapLines;
}

int32 UEWN_WidgetNavigationCursorHandler::GetForwardIndex( int32 CurrentIndex ) const
{
	UPanelWidget* PanelWidget = GetPanelWidget();
	if ( !ensure( PanelWidget ) )
	{
		return CurrentIndex;
	}

	const int32 ChildrenCount = PanelWidget->GetChildrenCount();

	const int32 StartIndex = CurrentIndex == INDEX_NONE ? 0 : CurrentIndex + 1;
	for ( int32 i = StartIndex; i < ChildrenCount; ++i )
	{
		if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( PanelWidget->GetChildAt( i ) ) )
		{
			return i;
		}
	}

	if ( IsLoopNavigation() )
	{
		int32 FirstIndex = [&]() -> int32
		{
			for ( int32 i = 0; i < StartIndex; ++i )
			{
				if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( PanelWidget->GetChildAt( i ) ) )
				{
					return i;
				}
			}
			return INDEX_NONE;
		}();
		if ( FirstIndex != INDEX_NONE )
		{
			return FirstIndex;
		}
	}

	return CurrentIndex;
}

int32 UEWN_WidgetNavigationCursorHandler::GetBackwardIndex( int32 CurrentIndex ) const
{
	UPanelWidget* PanelWidget = GetPanelWidget();
	if ( !ensure( PanelWidget ) )
	{
		return CurrentIndex;
	}

	const int32 ChildrenCount = PanelWidget->GetChildrenCount();

	const int32 StartIndex = CurrentIndex == INDEX_NONE ? ChildrenCount - 1 : CurrentIndex - 1;
	for ( int32 i = StartIndex; i >= 0; --i )
	{
		if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( PanelWidget->GetChildAt( i ) ) )
		{
			return i;
		}
	}

	if ( IsLoopNavigation() )
	{
		int32 LastIndex = [&]() -> int32
		{
			for ( int32 i = ChildrenCount - 1; i > StartIndex; --i )
			{
				if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( PanelWidget->GetChildAt( i ) ) )
				{
					return i;
				}
			}
			return INDEX_NONE;
		}();
		if ( LastIndex != INDEX_NONE )
		{
			return LastIndex;
		}
	}

	return CurrentIndex;
}
