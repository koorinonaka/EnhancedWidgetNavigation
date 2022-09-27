// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationConnector.h"

//
#include "Components/Widget.h"

//
#include "EWN_WidgetInputSubsystem.h"
#include "Navigation/EWN_WidgetNavigation.h"
#include "Navigation/EWN_WidgetNavigationHelper.h"

EEWN_WidgetInputType UEWN_WidgetNavigationConnector::TickNavigation( float DeltaTime )
{
	auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
	if ( !WidgetInputSubsystem )
	{
		return EEWN_WidgetInputType::None;
	}

	switch ( WidgetInputSubsystem->GetCurrentInputMode() )
	{
	case EEWN_WidgetInputMode::Mouse:
	{
		ForEachWidgetNavigation(
			[&]( UEWN_WidgetNavigation* WidgetNavigation )
			{
				// move cursor to inactive widget with mouse.
				WidgetNavigation->MoveWithMouseCursor();
			} );
	}
	break;
	}

	if ( ActiveIndex != INDEX_NONE )
	{
		EEWN_WidgetInputType InputResult = WidgetNavigations[ActiveIndex]->TickNavigation( DeltaTime );
		if ( InputResult != EEWN_WidgetInputType::None )
		{
			return InputResult;
		}
	}

	return EEWN_WidgetInputType::None;
}

int32 UEWN_WidgetNavigationConnector::GetActiveIndex() const
{
	return ActiveIndex;
}

bool UEWN_WidgetNavigationConnector::IsActive( TScriptInterface<IEWN_Interface_WidgetNavigation> INavigation ) const
{
	return ActiveIndex != INDEX_NONE ? WidgetNavigations[ActiveIndex] == INavigation : false;
}

TScriptInterface<IEWN_Interface_WidgetNavigation> UEWN_WidgetNavigationConnector::GetNavigationAt( int32 Index ) const
{
	return Index < WidgetNavigations.Num() ? WidgetNavigations[Index] : nullptr;
}

TArray<TScriptInterface<IEWN_Interface_WidgetNavigation>> UEWN_WidgetNavigationConnector::GetAllNavigations() const
{
	return WidgetNavigations;
}

void UEWN_WidgetNavigationConnector::Register( TScriptInterface<IEWN_Interface_WidgetNavigation> INavigation )
{
	if ( ensureAlways( INavigation ) )
	{
		INavigation->NavigationUpdatedDelegate.AddUObject( this, &ThisClass::OnNavigationUpdated );
		INavigation->SetMoveFocusFallback(
			IEWN_Interface_WidgetNavigation::FMoveFocusDelegate::CreateUObject( this, &ThisClass::MoveFocusFallback ) );
		WidgetNavigations.Emplace( INavigation );
	}
}

void UEWN_WidgetNavigationConnector::Unregister( TScriptInterface<IEWN_Interface_WidgetNavigation> INavigation )
{
	if ( ensureAlways( INavigation ) )
	{
		INavigation->NavigationUpdatedDelegate.RemoveAll( this );
		INavigation->SetMoveFocusFallback( IEWN_Interface_WidgetNavigation::FMoveFocusDelegate() );
		WidgetNavigations.Remove( INavigation );
	}
}

void UEWN_WidgetNavigationConnector::AddRoute( EEWN_WidgetCursor WidgetCursor,
	TScriptInterface<IEWN_Interface_WidgetNavigation> Source, TScriptInterface<IEWN_Interface_WidgetNavigation> Destination )
{
	if ( ensureAlways( Source && Destination ) )
	{
		FWidgetNavigationOverride Key;
		Key.NavigationObject = Source.GetObject();
		Key.WidgetCursor = WidgetCursor;

		if ( WidgetNavigationOverrides.Num( Key ) == 0 )
		{
			Source->SetMoveFocusOverride(
				IEWN_Interface_WidgetNavigation::FMoveFocusDelegate::CreateUObject( this, &ThisClass::MoveFocusOverride ) );
		}

		WidgetNavigationOverrides.Emplace( Key, Destination.GetObject() );
	}
}

void UEWN_WidgetNavigationConnector::RemoveRoute( EEWN_WidgetCursor WidgetCursor,
	TScriptInterface<IEWN_Interface_WidgetNavigation> Source, TScriptInterface<IEWN_Interface_WidgetNavigation> Destination )
{
	if ( ensureAlways( Source && Destination ) )
	{
		FWidgetNavigationOverride Key;
		Key.NavigationObject = Source.GetObject();
		Key.WidgetCursor = WidgetCursor;

		WidgetNavigationOverrides.Remove( Key, Destination.GetObject() );

		if ( WidgetNavigationOverrides.Num( Key ) == 0 )
		{
			Source->SetMoveFocusOverride( IEWN_Interface_WidgetNavigation::FMoveFocusDelegate() );
		}
	}
}

bool UEWN_WidgetNavigationConnector::MoveFocusOverride(
	IEWN_Interface_WidgetNavigation* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	if ( TryMoveFocusOverride( WidgetCursor, bFromOperation ) )
	{
		return true;
	}

	using namespace EWN::WidgetNavigation;

	FWidgetNavigationOverride Key;
	Key.NavigationObject = Cast<UObject>( INavigation );
	Key.WidgetCursor = WidgetCursor;

	if ( WidgetNavigationOverrides.Contains( Key ) )
	{
		if ( INavigation->TestFocus( WidgetCursor ) )
		{
			// default navigation worked, so use it
			return false;
		}

		UWidget* CurrentWidget = INavigation->GetCurrentWidget();
		check( CurrentWidget );

		TArray<TWeakObjectPtr<UObject>> Destinations;
		WidgetNavigationOverrides.MultiFind( Key, Destinations );

		TMap<UWidget*, FWidgetWithNavigation> WidgetsWithNavigation;
		for ( TWeakObjectPtr<UObject> Destination : Destinations )
		{
			auto* INavigationDest = Cast<IEWN_Interface_WidgetNavigation>( Destination.Get() );
			if ( !INavigationDest )
			{
				continue;
			}

			INavigationDest->ForEachWidgetNavigation(
				[&]( UEWN_WidgetNavigation* WidgetNavigation )
				{
					WidgetNavigation->ForEachFocusable(
						[&]( int32 i, UWidget* ChildWidget )
						{
							FWidgetWithNavigation& Info = WidgetsWithNavigation.Emplace( ChildWidget );
							Info.Navigation = WidgetNavigation;
							Info.Index = i;
							Info.Position = FHelper::GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
						} );
				} );
		}

		// find the closest widget in registered navigation
		if ( UWidget* NearestWidget = FHelper::FindFocusToNearest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[NearestWidget];
			WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
			return true;
		}
	}

	return false;
}

bool UEWN_WidgetNavigationConnector::MoveFocusFallback(
	IEWN_Interface_WidgetNavigation* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	if ( TryMoveFocusFallback( WidgetCursor, bFromOperation ) )
	{
		return true;
	}

	using namespace EWN::WidgetNavigation;

	UWidget* CurrentWidget = INavigation->GetCurrentWidget();
	check( CurrentWidget );

	TMap<UWidget*, FWidgetWithNavigation> WidgetsWithNavigation;
	ForEachWidgetNavigation(
		[&]( UEWN_WidgetNavigation* WidgetNavigation )
		{
			if ( WidgetNavigation->bIndependentNavigation || WidgetNavigation->GetChildIndex( CurrentWidget ) != INDEX_NONE )
			{
				// ignore current widget navigation
				return;
			}

			WidgetNavigation->ForEachFocusable(
				[&]( int32 i, UWidget* ChildWidget )
				{
					FWidgetWithNavigation& Info = WidgetsWithNavigation.Emplace( ChildWidget );
					Info.Navigation = WidgetNavigation;
					Info.Index = i;
					Info.Position = FHelper::GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
				} );
		} );

	// fallback if MoveFocus fails
	if ( UWidget* NearestWidget = FHelper::FindFocusToNearest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
	{
		const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[NearestWidget];
		WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
		return true;
	}
	else if ( bLoopNavigation )
	{
		if ( UWidget* FarthestWidget = FHelper::FindFocusToOpposite( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[FarthestWidget];
			WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
			return true;
		}
	}

	return false;
}

void UEWN_WidgetNavigationConnector::OnNavigationUpdated(
	IEWN_Interface_WidgetNavigation* INavigation, int32 OldIndex, int32 NewIndex, bool bFromOperation )
{
	if ( NewIndex != INDEX_NONE )
	{
		const int32 NavigationIndex = WidgetNavigations.IndexOfByKey( Cast<UObject>( INavigation ) );
		check( NavigationIndex != INDEX_NONE );

		if ( NavigationIndex != ActiveIndex )
		{
			int32 OldActiveIndex = ActiveIndex;
			ActiveIndex = NavigationIndex;

			for ( int32 i = 0; i < WidgetNavigations.Num(); ++i )
			{
				if ( i != ActiveIndex )
				{
					// WidgetSwitcher has only one focus.
					WidgetNavigations[i]->InvalidateNavigation();
				}
			}

			NavigationUpdatedDelegate.Broadcast( this, OldActiveIndex, ActiveIndex, bFromOperation );
		}
	}
}

UWidget* UEWN_WidgetNavigationConnector::GetCurrentWidget() const
{
	return ActiveIndex != INDEX_NONE ? WidgetNavigations[ActiveIndex]->GetCurrentWidget() : nullptr;
}

bool UEWN_WidgetNavigationConnector::TestFocus( EEWN_WidgetCursor WidgetCursor ) const
{
	return ActiveIndex != INDEX_NONE ? WidgetNavigations[ActiveIndex]->TestFocus( WidgetCursor ) : false;
}

void UEWN_WidgetNavigationConnector::ForEachWidgetNavigation( const TFunctionRef<void( UEWN_WidgetNavigation* )> Callback )
{
	if ( IsNavigationEnabled() )
	{
		for ( TScriptInterface<IEWN_Interface_WidgetNavigation> WidgetNavigation : WidgetNavigations )
		{
			WidgetNavigation->ForEachWidgetNavigation( Callback );
		}
	}
}

void UEWN_WidgetNavigationConnector::InvalidateNavigation()
{
	for ( TScriptInterface<IEWN_Interface_WidgetNavigation> WidgetNavigation : WidgetNavigations )
	{
		WidgetNavigation->InvalidateNavigation();
	}

	ActiveIndex = INDEX_NONE;
}
