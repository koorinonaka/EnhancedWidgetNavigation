// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"
#include "Navigation/EWN_WidgetNavigationHelper.h"

namespace EWN::WidgetNavigation
{
class FCursorHandler_Grid::FDistanceBasedNav
{
	FCursorHandler_Grid& OwnerHandler;

public:
	explicit FDistanceBasedNav( FCursorHandler_Grid& Owner ) : OwnerHandler( Owner ) {}

	int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const
	{
		int32 ResultIndex = CurrentIndex;

		UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget();
		if ( !PanelWidget )
		{
			return ResultIndex;
		}

		UWidget* CurrentWidget = PanelWidget->GetChildAt( CurrentIndex );
		check( CurrentWidget );

		TMap<UWidget*, FWidgetWithNavigation> WidgetsWithNavigation;
		FHelper::ForEachPanelChildren( PanelWidget,
			[&]( int32 i, UWidget* ChildWidget )
			{
				if ( ChildWidget != CurrentWidget && IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( ChildWidget ) )
				{
					FWidgetWithNavigation& Info = WidgetsWithNavigation.Emplace( ChildWidget );
					Info.Index = i;
					Info.Position = FHelper::GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
				}
			} );

		// fallback if MoveFocus fails
		if ( const UWidget* NearestWidget = FHelper::FindFocusToNearest( *CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			ResultIndex = WidgetsWithNavigation[NearestWidget].Index;
		}
		else if ( OwnerHandler.IsLoopNavigation() && !bLoopIgnored )
		{
			if ( const UWidget* FarthestWidget =
					 FHelper::FindFocusToOpposite( *CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
			{
				ResultIndex = WidgetsWithNavigation[FarthestWidget].Index;
			}
		}

		return ResultIndex;
	}
};

void FCursorHandler_Grid::InitDistanceBased()
{
	DistanceBasedNav = MakePimpl<FDistanceBasedNav>( *this );
}

int32 FCursorHandler_Grid::GetNextIndexFromDistanceBased(
	int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const
{
	return DistanceBasedNav->GetNextIndex( CurrentIndex, WidgetCursor, bLoopIgnored );
}
}	 // namespace EWN::WidgetNavigation
