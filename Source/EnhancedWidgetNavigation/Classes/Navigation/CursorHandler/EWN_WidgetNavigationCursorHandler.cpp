// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler.h"

#include "EWN_WidgetNavigationCursorHandler_Grid.h"
#include "EWN_WidgetNavigationCursorHandler_Horizontal.h"
#include "EWN_WidgetNavigationCursorHandler_Vertical.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"
#include "Navigation/EWN_WidgetNavigation.h"

namespace EWN::WidgetNavigation
{
TSharedPtr<FCursorHandler> FCursorFactory::CreateHandler( UEWN_WidgetNavigation* Navigation )
{
	if ( auto* PanelWidget = Navigation->GetTypedOuter<UPanelWidget>(); PanelWidget && !PanelWidget->IsDesignTime() )
	{
		if ( FCursorHandler_Vertical::IsVertical( PanelWidget ) )
		{
			return MakeShared<FCursorHandler_Vertical>( Navigation );
		}
		if ( FCursorHandler_Horizontal::IsHorizontal( PanelWidget ) )
		{
			return MakeShared<FCursorHandler_Horizontal>( Navigation );
		}
		if ( FCursorHandler_Grid::IsGrid( PanelWidget ) )
		{
			return MakeShared<FCursorHandler_Grid>( Navigation );
		}
		// fallback for PanelWidget that is not navigative like SizeBox.
		return MakeShared<FCursorHandler_Default>( Navigation );
	}

	return nullptr;
}

FCursorHandler::FCursorHandler( UEWN_WidgetNavigation* Navigation )
{
	OuterNavigation = Navigation;
}

UPanelWidget* FCursorHandler::GetPanelWidget() const
{
	return OuterNavigation->GetTypedOuter<UPanelWidget>();
}

bool FCursorHandler::IsLoopNavigation() const
{
	return OuterNavigation->bLoopNavigation;
}

bool FCursorHandler::IsDistanceBasedNavigation() const
{
	return OuterNavigation->bDistanceBasedNavigation;
}

bool FCursorHandler::IsWrapLines() const
{
	return OuterNavigation->bWrapLines;
}

int32 FCursorHandler::GetForwardIndex( int32 CurrentIndex, bool bLoopIgnored ) const
{
	const UPanelWidget* PanelWidget = GetPanelWidget();
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

	if ( IsLoopNavigation() && !bLoopIgnored )
	{
		if ( const int32 FirstIndex = [&]() -> int32
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
			 FirstIndex != INDEX_NONE )
		{
			return FirstIndex;
		}
	}

	return CurrentIndex;
}

int32 FCursorHandler::GetBackwardIndex( int32 CurrentIndex, bool bLoopIgnored ) const
{
	const UPanelWidget* PanelWidget = GetPanelWidget();
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

	if ( IsLoopNavigation() && !bLoopIgnored )
	{
		if ( const int32 LastIndex = [&]() -> int32
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
			 LastIndex != INDEX_NONE )
		{
			return LastIndex;
		}
	}

	return CurrentIndex;
}
}	 // namespace EWN::WidgetNavigation
