// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationHelper.h"

namespace EWN::WidgetNavigation
{
int32 FHelper::FindPanelIndex( const UPanelWidget* PanelWidget, const TFunctionRef<bool( int32, UWidget* )>& Callback )
{
	if ( ensure( PanelWidget ) )
	{
		const int32 ChildrenCount = PanelWidget->GetChildrenCount();
		for ( int32 i = 0; i < ChildrenCount; ++i )
		{
			if ( UWidget* ChildWidget = PanelWidget->GetChildAt( i ); Callback( i, ChildWidget ) )
			{
				return i;
			}
		}
	}

	return INDEX_NONE;
}

void FHelper::ForEachPanelChildren( const UPanelWidget* PanelWidget, const TFunctionRef<void( int32, UWidget* )>& Callback )
{
	if ( ensure( PanelWidget ) )
	{
		const int32 ChildrenCount = PanelWidget->GetChildrenCount();
		for ( int32 i = 0; i < ChildrenCount; ++i )
		{
			Callback( i, PanelWidget->GetChildAt( i ) );
		}
	}
}

FVector2D FHelper::GetCursorPosition( const FGeometry& Geometry, const EEWN_WidgetCursor WidgetCursor )
{
	FVector2D Position = Geometry.GetAbsolutePosition();
	const FVector2D Size = Geometry.GetAbsoluteSize();

	switch ( WidgetCursor )
	{
	case EEWN_WidgetCursor::Up: Position += FVector2D( Size.X * .5f, 0.f ); break;
	case EEWN_WidgetCursor::Down: Position += FVector2D( Size.X * .5f, Size.Y ); break;
	case EEWN_WidgetCursor::Left: Position += FVector2D( 0.f, Size.Y * .5f ); break;
	case EEWN_WidgetCursor::Right: Position += FVector2D( Size.X, Size.Y * .5f ); break;
	default: Position += FVector2D( Size.X * .5f, Size.Y * .5f ); break;
	}

	return Position;
}

namespace
{
struct FWidgetSortInfo
{
	float DistSquared = 0.f;
	bool bExtLinesIntersect = false;
};
}

UWidget* FHelper::FindFocusToNearest( const UWidget& CurrentWidget, const EEWN_WidgetCursor WidgetCursor,
	const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation )
{
	UWidget* FoundWidget = nullptr;

	const FVector2D& SourcePosition = GetCursorPosition( CurrentWidget.GetCachedGeometry(), WidgetCursor );

	FVector2D SourceP1, SourceP2, SourceP3, SourceP4;
	{
		const FGeometry& Geometry = CurrentWidget.GetCachedGeometry();
		const FVector2D Position = Geometry.GetAbsolutePosition();
		const FVector2D Size = Geometry.GetAbsoluteSize();

		SourceP1 = Position;
		SourceP2 = Position + FVector2D( Size.X, 0.f );
		SourceP3 = Position + FVector2D( 0.f, Size.Y );
		SourceP4 = Position + FVector2D( Size.X, Size.Y );
	}

	TMap<TObjectPtr<UWidget>, FWidgetSortInfo> SortInfoMap;

	TArray<UWidget*> Widgets;
	WidgetsWithNavigation.GetKeys( Widgets );

	Widgets.RemoveAll(
		[&]( UWidget* Widget )
		{
			const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[Widget];
			const FVector2D SourceToTarget = WidgetInfo.Position - SourcePosition;

			const bool bMatchCursor = [&]
			{
				bool bResult = false;
				switch ( WidgetCursor )
				{
				case EEWN_WidgetCursor::Up: bResult = SourceToTarget.Y < 0.f; break;
				case EEWN_WidgetCursor::Down: bResult = SourceToTarget.Y > 0.f; break;
				case EEWN_WidgetCursor::Left: bResult = SourceToTarget.X < 0.f; break;
				case EEWN_WidgetCursor::Right: bResult = SourceToTarget.X > 0.f; break;
				default:;
				}
				return bResult;
			}();
			if ( bMatchCursor )
			{
				FWidgetSortInfo& SortInfo = SortInfoMap.Emplace( Widget );
				SortInfo.DistSquared = SourceToTarget.SizeSquared();

				FVector2D TargetP1, TargetP2, TargetP3, TargetP4;
				{
					const FGeometry& Geometry = Widget->GetCachedGeometry();
					const FVector2D Position = Geometry.GetAbsolutePosition();
					const FVector2D Size = Geometry.GetAbsoluteSize();

					TargetP1 = Position;
					TargetP2 = Position + FVector2D( Size.X, 0.f );
					TargetP3 = Position + FVector2D( 0.f, Size.Y );
					TargetP4 = Position + FVector2D( Size.X, Size.Y );
				}

				// checks if either end of the geometry intersects.
				// add 1.f due to miscalculation.
				{
					switch ( WidgetCursor )
					{
					case EEWN_WidgetCursor::Up:
					case EEWN_WidgetCursor::Down:
					{
						SortInfo.bExtLinesIntersect = ( TargetP1.X < SourceP1.X + 1.f && SourceP1.X - 1.f < TargetP2.X ) ||
													  ( TargetP1.X < SourceP2.X + 1.f && SourceP2.X - 1.f < TargetP2.X );
					}
					break;

					case EEWN_WidgetCursor::Left:
					case EEWN_WidgetCursor::Right:
					{
						SortInfo.bExtLinesIntersect = ( TargetP1.Y < SourceP1.Y + 1.f && SourceP1.Y - 1.f < TargetP3.Y ) ||
													  ( TargetP1.Y < SourceP3.Y + 1.f && SourceP3.Y - 1.f < TargetP3.Y );
					}
					break;

					default:;
					}
				}
			}

			return !bMatchCursor;
		} );
	if ( Widgets.Num() != 0 )
	{
		Widgets.Sort(
			[&]( UWidget& A, UWidget& B )
			{
				const FWidgetSortInfo& SortInfoA = SortInfoMap[&A];
				const FWidgetSortInfo& SortInfoB = SortInfoMap[&B];
				if ( SortInfoA.bExtLinesIntersect && !SortInfoB.bExtLinesIntersect )
				{
					return true;
				}
				if ( !SortInfoA.bExtLinesIntersect && SortInfoB.bExtLinesIntersect )
				{
					return false;
				}
				return SortInfoA.DistSquared < SortInfoB.DistSquared;
			} );
		FoundWidget = Widgets[0];
	}

	return FoundWidget;
}

UWidget* FHelper::FindFocusToOpposite( const UWidget& CurrentWidget, const EEWN_WidgetCursor WidgetCursor,
	const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation )
{
	UWidget* FoundWidget = nullptr;

	const FVector2D& SourcePosition = GetCursorPosition( CurrentWidget.GetCachedGeometry(), WidgetCursor );

	TArray<UWidget*> Widgets;
	WidgetsWithNavigation.GetKeys( Widgets );

	// step1: カーソルと反対側の一番遠いWidgetを探す
	float Farthest = 0.f;
	for ( const UWidget* Widget : Widgets )
	{
		const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[Widget];
		const FVector2D SourceToTarget = WidgetInfo.Position - SourcePosition;

		float Score = 0.f;

		switch ( WidgetCursor )
		{
		case EEWN_WidgetCursor::Up: Score = SourceToTarget.Y > 0.f ? SourceToTarget.Y : 0.f; break;
		case EEWN_WidgetCursor::Down: Score = SourceToTarget.Y < 0.f ? SourceToTarget.Y : 0.f; break;
		case EEWN_WidgetCursor::Left: Score = SourceToTarget.X > 0.f ? SourceToTarget.X : 0.f; break;
		case EEWN_WidgetCursor::Right: Score = SourceToTarget.X < 0.f ? SourceToTarget.X : 0.f; break;
		default:;
		}

		if ( FMath::Abs( Farthest ) < FMath::Abs( Score ) )
		{
			Farthest = Score;
		}
	}

	// step2: カーソルから基準点を決める
	FVector2D BasePosition;
	switch ( WidgetCursor )
	{
	case EEWN_WidgetCursor::Up: BasePosition = FVector2D( SourcePosition.X, SourcePosition.Y + Farthest ); break;
	case EEWN_WidgetCursor::Down: BasePosition = FVector2D( SourcePosition.X, SourcePosition.Y + Farthest ); break;
	case EEWN_WidgetCursor::Left: BasePosition = FVector2D( SourcePosition.X + Farthest, SourcePosition.Y ); break;
	case EEWN_WidgetCursor::Right: BasePosition = FVector2D( SourcePosition.X + Farthest, SourcePosition.Y ); break;
	default:;
	}

	// step3: 基準点から一番近いWidgetを選択
	float Nearest = FLT_MAX;
	for ( UWidget* Widget : Widgets )
	{
		const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[Widget];

		if ( const float DistSquared = FVector2D::DistSquared( WidgetInfo.Position, BasePosition ); DistSquared < Nearest )
		{
			FoundWidget = Widget;
			Nearest = DistSquared;
		}
	}

	return FoundWidget;
}
}	 // namespace EWN::WidgetNavigation
