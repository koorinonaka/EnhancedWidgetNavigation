// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"

namespace EWN::WidgetNavigation
{
class FCursorHandler_Grid::FSimpleNav
{
	FCursorHandler_Grid& OwnerHandler;

public:
	explicit FSimpleNav( FCursorHandler_Grid& Owner ) : OwnerHandler( Owner ) {}

	int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const
	{
		int32 ResultIndex = CurrentIndex;

		const UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget();
		if ( !PanelWidget )
		{
			return ResultIndex;
		}

		const FIntPoint FromPoint = GetGridPoint( CurrentIndex );

		TMap<FIntPoint, UWidget*> GridWidgets;
		FIntPoint LargestPoint = FIntPoint::ZeroValue;
		FillGridWidgets( GridWidgets, LargestPoint );

		auto ValidateSlot = [&]( const FIntPoint& Address )
		{
			if ( const UWidget* FoundWidget = GridWidgets.FindRef( Address ) )
			{
				if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( FoundWidget ) )
				{
					ResultIndex = PanelWidget->GetChildIndex( FoundWidget );
					return true;
				}
			}
			return false;
		};

		switch ( WidgetCursor )
		{
		case EEWN_WidgetCursor::Up:
		{
			SearchWithDecrement(
				FromPoint.Y - 1, LargestPoint.Y, FromPoint.X, LargestPoint.X,
				[&]( int32 SearchY, int32 SearchX ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); }, bLoopIgnored );
		}
		break;

		case EEWN_WidgetCursor::Down:
		{
			SearchWithIncrement(
				FromPoint.Y + 1, LargestPoint.Y, FromPoint.X, LargestPoint.X,
				[&]( int32 SearchY, int32 SearchX ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); }, bLoopIgnored );
		}
		break;

		case EEWN_WidgetCursor::Left:
		{
			SearchWithDecrement(
				FromPoint.X - 1, LargestPoint.X, FromPoint.Y, LargestPoint.Y,
				[&]( int32 SearchX, int32 SearchY ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); }, bLoopIgnored );
		}
		break;

		case EEWN_WidgetCursor::Right:
		{
			SearchWithIncrement(
				FromPoint.X + 1, LargestPoint.X, FromPoint.Y, LargestPoint.Y,
				[&]( int32 SearchX, int32 SearchY ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); }, bLoopIgnored );
		}
		break;

		default:;
		}

		return ResultIndex;
	}

private:
	FIntPoint GetGridPoint( int32 Index ) const
	{
		if ( const UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget() )
		{
			if ( const UWidget* ChildWidget = PanelWidget->GetChildAt( Index ) )
			{
				if ( auto* GridSlot = Cast<UGridSlot>( ChildWidget->Slot ) )
				{
#if EWN_UE_VERSION_OR_LATER( 5, 1 )
					return FIntPoint( GridSlot->GetColumn(), GridSlot->GetRow() );
#else
					return FIntPoint( GridSlot->Column, GridSlot->Row );
#endif
				}
				if ( auto* UniformGridSlot = Cast<UUniformGridSlot>( ChildWidget->Slot ) )
				{
#if EWN_UE_VERSION_OR_LATER( 5, 1 )
					return FIntPoint( UniformGridSlot->GetColumn(), UniformGridSlot->GetRow() );
#else
					return FIntPoint( UniformGridSlot->Column, UniformGridSlot->Row );
#endif
				}
			}
		}

		return FIntPoint::ZeroValue;
	}

	void FillGridWidgets( TMap<FIntPoint, UWidget*>& OutWidgets, FIntPoint& OutLargestPoint ) const
	{
		OutWidgets.Empty();
		OutLargestPoint = FIntPoint::ZeroValue;

		if ( const UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget() )
		{
			const int32 ChildrenCount = PanelWidget->GetChildrenCount();
			for ( int32 i = 0; i < ChildrenCount; ++i )
			{
				FIntPoint GridPoint = GetGridPoint( i );

				ensureAlways( !OutWidgets.Contains( GridPoint ) );
				OutWidgets.Emplace( GridPoint, PanelWidget->GetChildAt( i ) );

				OutLargestPoint.X = FMath::Max( OutLargestPoint.X, GridPoint.X );
				OutLargestPoint.Y = FMath::Max( OutLargestPoint.Y, GridPoint.Y );
			}
		}
	}

	void SearchWithIncrement( int32 From1, int32 Max1, int32 From2, int32 Max2, const TFunctionRef<bool( int32, int32 )>& Callback,
		bool bLoopIgnored ) const
	{
		for ( int32 i = From1; i <= Max1; ++i )
		{
			if ( Callback( i, From2 ) )
			{
				return;
			}
		}

		if ( OwnerHandler.IsWrapLines() )
		{
			for ( int32 j = From2 + 1; j <= Max2; ++j )
			{
				for ( int32 i = 0; i <= Max1; ++i )
				{
					if ( Callback( i, j ) )
					{
						return;
					}
				}
			}

			if ( bLoopIgnored )
			{
				return;
			}

			for ( int32 j = 0; j <= From2; ++j )
			{
				for ( int32 i = 0; i <= Max1; ++i )
				{
					if ( Callback( i, j ) )
					{
						return;
					}
				}
			}
		}
		else if ( OwnerHandler.IsLoopNavigation() )
		{
			if ( bLoopIgnored )
			{
				return;
			}

			for ( int32 i = 0; i < From1; ++i )
			{
				if ( Callback( i, From2 ) )
				{
					return;
				}
			}
		}
		else
		{
			const int32 MaxLoop = FMath::Max( From2, Max2 - From2 );
			for ( int32 i = 1; i <= MaxLoop; ++i )
			{
				const int32 PrevI = From2 - i;
				const int32 NextI = From2 + i;

				if ( PrevI >= 0 && Callback( From1, PrevI ) )
				{
					return;
				}
				if ( NextI <= Max2 && Callback( From1, NextI ) )
				{
					return;
				}
			}
		}
	}

	void SearchWithDecrement( int32 From1, int32 Max1, int32 From2, int32 Max2, const TFunctionRef<bool( int32, int32 )>& Callback,
		bool bLoopIgnored ) const
	{
		for ( int32 i = From1; i >= 0; --i )
		{
			if ( Callback( i, From2 ) )
			{
				return;
			}
		}

		if ( OwnerHandler.IsWrapLines() )
		{
			for ( int32 j = From2 - 1; j >= 0; --j )
			{
				for ( int32 i = Max1; i >= 0; --i )
				{
					if ( Callback( i, j ) )
					{
						return;
					}
				}
			}

			if ( bLoopIgnored )
			{
				return;
			}

			for ( int32 j = Max2; j >= From2; --j )
			{
				for ( int32 i = Max1; i >= 0; --i )
				{
					if ( Callback( i, j ) )
					{
						return;
					}
				}
			}
		}
		else if ( OwnerHandler.IsLoopNavigation() )
		{
			if ( bLoopIgnored )
			{
				return;
			}

			for ( int32 i = Max1; i > From1; --i )
			{
				if ( Callback( i, From2 ) )
				{
					return;
				}
			}
		}
		else
		{
			const int32 MaxLoop = FMath::Max( From2, Max2 - From2 );
			for ( int32 i = 1; i <= MaxLoop; ++i )
			{
				const int32 PrevI = From2 - i;
				const int32 NextI = From2 + i;

				if ( PrevI >= 0 && Callback( From1, PrevI ) )
				{
					return;
				}
				if ( NextI <= Max2 && Callback( From1, NextI ) )
				{
					return;
				}
			}
		}
	}
};

void FCursorHandler_Grid::InitSimple()
{
	SimpleNav = MakePimpl<FSimpleNav>( *this );
}

int32 FCursorHandler_Grid::GetNextIndexFromSimple( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const
{
	return SimpleNav->GetNextIndex( CurrentIndex, WidgetCursor, bLoopIgnored );
}
}	 // namespace EWN::WidgetNavigation
