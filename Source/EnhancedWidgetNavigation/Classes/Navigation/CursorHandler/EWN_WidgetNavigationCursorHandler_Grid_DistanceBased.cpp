// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"

//
#include "Navigation/EWN_WidgetNavigationHelper.h"

class FEWN_WidgetNavigationCursorHandler_Grid::FDistanceBasedNav
{
	FEWN_WidgetNavigationCursorHandler_Grid& OwnerHandler;

public:
	FDistanceBasedNav( FEWN_WidgetNavigationCursorHandler_Grid& Owner ) : OwnerHandler( Owner ) {}

	int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
	{
		using namespace EWN::WidgetNavigationHelper;

		int32 ResultIndex = CurrentIndex;

		UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget();
		if ( !PanelWidget )
		{
			return ResultIndex;
		}

		UWidget* CurrentWidget = PanelWidget->GetChildAt( CurrentIndex );
		check( CurrentWidget );

		TMap<UWidget*, FWidgetWithNavigation> WidgetsWithNavigation;
		EWN::WidgetNavigationHelper::ForEachPanelChildren( PanelWidget,
			[&]( int32 i, UWidget* ChildWidget )
			{
				if ( ChildWidget != CurrentWidget && IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( ChildWidget ) )
				{
					FWidgetWithNavigation& Info = WidgetsWithNavigation.Emplace( ChildWidget );
					Info.Index = i;
					Info.Position = GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
				}
			} );

		// fallback if MoveFocus fails
		if ( UWidget* NearestWidget = FindFocusToNearest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			ResultIndex = WidgetsWithNavigation[NearestWidget].Index;
		}
		else if ( OwnerHandler.IsLoopNavigation() )
		{
			if ( UWidget* FarthestWidget = FindFocusToOpposite( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
			{
				ResultIndex = WidgetsWithNavigation[FarthestWidget].Index;
			}
		}

		return ResultIndex;
	}
};

void FEWN_WidgetNavigationCursorHandler_Grid::InitDistanceBased()
{
	DistanceBasedNav = MakePimpl<FDistanceBasedNav>( *this );
}

int32 FEWN_WidgetNavigationCursorHandler_Grid::GetNextIndexFromDistanceBased(
	int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
{
	return DistanceBasedNav->GetNextIndex( CurrentIndex, WidgetCursor );
}