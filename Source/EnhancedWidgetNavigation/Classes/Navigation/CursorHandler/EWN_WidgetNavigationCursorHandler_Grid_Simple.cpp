// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationCursorHandler_Grid.h"

class FEWN_WidgetNavigationCursorHandler_Grid::FSimpleNav
{
	FEWN_WidgetNavigationCursorHandler_Grid& OwnerHandler;

public:
	FSimpleNav( FEWN_WidgetNavigationCursorHandler_Grid& Owner ) : OwnerHandler( Owner ) {}

	int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
	{
		int32 ResultIndex = CurrentIndex;

		UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget();
		if ( !PanelWidget )
		{
			return ResultIndex;
		}

		FIntPoint FromPoint = GetGridPoint( CurrentIndex );

		TMap<FIntPoint, UWidget*> GridWidgets;
		FIntPoint LargestPoint = FIntPoint::ZeroValue;
		FillGridWidgets( GridWidgets, LargestPoint );

		auto ValidateSlot = [&]( const FIntPoint& Address )
		{
			if ( UWidget* FoundWidget = GridWidgets.FindRef( Address ) )
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
			SearchWithDecrement( FromPoint.Y - 1, LargestPoint.Y, FromPoint.X, LargestPoint.X,
				[&]( int32 SearchY, int32 SearchX ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); } );
		}
		break;

		case EEWN_WidgetCursor::Down:
		{
			SearchWithIncrement( FromPoint.Y + 1, LargestPoint.Y, FromPoint.X, LargestPoint.X,
				[&]( int32 SearchY, int32 SearchX ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); } );
		}
		break;

		case EEWN_WidgetCursor::Left:
		{
			SearchWithDecrement( FromPoint.X - 1, LargestPoint.X, FromPoint.Y, LargestPoint.Y,
				[&]( int32 SearchX, int32 SearchY ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); } );
		}
		break;

		case EEWN_WidgetCursor::Right:
		{
			SearchWithIncrement( FromPoint.X + 1, LargestPoint.X, FromPoint.Y, LargestPoint.Y,
				[&]( int32 SearchX, int32 SearchY ) { return ValidateSlot( FIntPoint( SearchX, SearchY ) ); } );
		}
		break;
		}

		return ResultIndex;
	}

private:
	FIntPoint GetGridPoint( int32 Index ) const
	{
		if ( UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget() )
		{
			UWidget* ChildWidget = PanelWidget->GetChildAt( Index );
			if ( auto* GridSlot = Cast<UGridSlot>( ChildWidget->Slot ) )
			{
				return FIntPoint( GridSlot->Column, GridSlot->Row );
			}
			else if ( auto* UniformGridSlot = Cast<UUniformGridSlot>( ChildWidget->Slot ) )
			{
				return FIntPoint( UniformGridSlot->Column, UniformGridSlot->Row );
			}
		}
		return FIntPoint::ZeroValue;
	}

	void FillGridWidgets( TMap<FIntPoint, UWidget*>& OutWidgets, FIntPoint& OutLargestPoint ) const
	{
		OutWidgets.Empty();
		OutLargestPoint = FIntPoint::ZeroValue;

		if ( UPanelWidget* PanelWidget = OwnerHandler.GetPanelWidget() )
		{
			int32 ChildrenCount = PanelWidget->GetChildrenCount();
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

	void SearchWithIncrement(
		int32 From1, int32 Max1, int32 From2, int32 Max2, const TFunctionRef<bool( int32, int32 )> Callback ) const
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
			int32 MaxLoop = FMath::Max( From2, Max2 - From2 );
			for ( int32 i = 1; i <= MaxLoop; ++i )
			{
				int32 PrevI = From2 - i;
				int32 NextI = From2 + i;

				if ( PrevI >= 0 && Callback( From1, PrevI ) )
				{
					return;
				}
				else if ( NextI <= Max2 && Callback( From1, NextI ) )
				{
					return;
				}
			}
		}
	}

	void SearchWithDecrement(
		int32 From1, int32 Max1, int32 From2, int32 Max2, const TFunctionRef<bool( int32, int32 )> Callback ) const
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
			int32 MaxLoop = FMath::Max( From2, Max2 - From2 );
			for ( int32 i = 1; i <= MaxLoop; ++i )
			{
				int32 PrevI = From2 - i;
				int32 NextI = From2 + i;

				if ( PrevI >= 0 && Callback( From1, PrevI ) )
				{
					return;
				}
				else if ( NextI <= Max2 && Callback( From1, NextI ) )
				{
					return;
				}
			}
		}
	}
};

void FEWN_WidgetNavigationCursorHandler_Grid::InitSimple()
{
	SimpleNav = MakePimpl<FSimpleNav>( *this );
}

int32 FEWN_WidgetNavigationCursorHandler_Grid::GetNextIndexFromSimple( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const
{
	return SimpleNav->GetNextIndex( CurrentIndex, WidgetCursor );
}