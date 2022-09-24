// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationSwitcher.h"

//
#include "EWN_WidgetInputSubsystem.h"
#include "Navigation/CursorHandler/EWN_WidgetNavigationCursorHandler.h"
#include "Navigation/EWN_WidgetNavigation.h"
#include "Navigation/EWN_WidgetNavigationHelper.h"

EEWN_WidgetInputType UEWN_WidgetNavigationSwitcher::TickNavigation( float DeltaTime )
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

int32 UEWN_WidgetNavigationSwitcher::GetActiveIndex() const
{
	return ActiveIndex;
}

bool UEWN_WidgetNavigationSwitcher::IsActive( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> INavigation ) const
{
	return ActiveIndex != INDEX_NONE ? WidgetNavigations[ActiveIndex] == INavigation : false;
}

TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> UEWN_WidgetNavigationSwitcher::GetNavigationAt( int32 Index ) const
{
	return Index < WidgetNavigations.Num() ? WidgetNavigations[Index] : nullptr;
}

TArray<TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher>> UEWN_WidgetNavigationSwitcher::GetAllNavigations() const
{
	return WidgetNavigations;
}

void UEWN_WidgetNavigationSwitcher::Register( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> INavigation )
{
	if ( ensureAlways( INavigation ) )
	{
		INavigation->NavigationUpdatedDelegate.AddUObject( this, &ThisClass::OnNavigationUpdated );
		INavigation->SetMoveFocusFallback(
			IEWN_Interface_WidgetNavigationSwitcher::FMoveFocusDelegate::CreateUObject( this, &ThisClass::MoveFocusFallback ) );
		WidgetNavigations.Emplace( INavigation );
	}
}

void UEWN_WidgetNavigationSwitcher::Unregister( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> INavigation )
{
	if ( ensureAlways( INavigation ) )
	{
		INavigation->NavigationUpdatedDelegate.RemoveAll( this );
		INavigation->SetMoveFocusFallback( IEWN_Interface_WidgetNavigationSwitcher::FMoveFocusDelegate() );
		WidgetNavigations.Remove( INavigation );
	}
}

void UEWN_WidgetNavigationSwitcher::AddRoute( EEWN_WidgetCursor WidgetCursor,
	TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Source,
	TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Destination )
{
	if ( ensureAlways( Source && Destination ) )
	{
		FWidgetNavigationOverride Key;
		Key.NavigationObject = Source.GetObject();
		Key.WidgetCursor = WidgetCursor;

		if ( WidgetNavigationOverrides.Num( Key ) == 0 )
		{
			Source->SetMoveFocusOverride(
				IEWN_Interface_WidgetNavigationSwitcher::FMoveFocusDelegate::CreateUObject( this, &ThisClass::MoveFocusOverride ) );
		}

		WidgetNavigationOverrides.Emplace( Key, Destination.GetObject() );
	}
}

void UEWN_WidgetNavigationSwitcher::RemoveRoute( EEWN_WidgetCursor WidgetCursor,
	TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Source,
	TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Destination )
{
	if ( ensureAlways( Source && Destination ) )
	{
		FWidgetNavigationOverride Key;
		Key.NavigationObject = Source.GetObject();
		Key.WidgetCursor = WidgetCursor;

		WidgetNavigationOverrides.Remove( Key, Destination.GetObject() );

		if ( WidgetNavigationOverrides.Num( Key ) == 0 )
		{
			Source->SetMoveFocusOverride( IEWN_Interface_WidgetNavigationSwitcher::FMoveFocusDelegate() );
		}
	}
}

bool UEWN_WidgetNavigationSwitcher::MoveFocusOverride(
	IEWN_Interface_WidgetNavigationSwitcher* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	if ( TryMoveFocusOverride( WidgetCursor, bFromOperation ) )
	{
		return true;
	}

	using namespace EWN::WidgetNavigationHelper;

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
			auto* INavigationDest = Cast<IEWN_Interface_WidgetNavigationSwitcher>( Destination.Get() );
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
							Info.Position = GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
						} );
				} );
		}

		// find the closest widget in registered navigation
		if ( UWidget* NearestWidget = FindFocusToNearest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[NearestWidget];
			WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
			return true;
		}
	}

	return false;
}

bool UEWN_WidgetNavigationSwitcher::MoveFocusFallback(
	IEWN_Interface_WidgetNavigationSwitcher* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	if ( TryMoveFocusFallback( WidgetCursor, bFromOperation ) )
	{
		return true;
	}

	using namespace EWN::WidgetNavigationHelper;

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
					Info.Position = GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
				} );
		} );

	// fallback if MoveFocus fails
	if ( UWidget* NearestWidget = FindFocusToNearest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
	{
		const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[NearestWidget];
		WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
		return true;
	}
	else if ( bLoopNavigation )
	{
		if ( UWidget* FarthestWidget = FindFocusToOpposite( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[FarthestWidget];
			WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
			return true;
		}
	}

	return false;
}

void UEWN_WidgetNavigationSwitcher::OnNavigationUpdated(
	IEWN_Interface_WidgetNavigationSwitcher* INavigation, int32 OldIndex, int32 NewIndex, bool bFromOperation )
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

UWidget* UEWN_WidgetNavigationSwitcher::GetCurrentWidget() const
{
	return ActiveIndex != INDEX_NONE ? WidgetNavigations[ActiveIndex]->GetCurrentWidget() : nullptr;
}

bool UEWN_WidgetNavigationSwitcher::TestFocus( EEWN_WidgetCursor WidgetCursor ) const
{
	return ActiveIndex != INDEX_NONE ? WidgetNavigations[ActiveIndex]->TestFocus( WidgetCursor ) : false;
}

void UEWN_WidgetNavigationSwitcher::ForEachWidgetNavigation( const TFunctionRef<void( UEWN_WidgetNavigation* )> Callback )
{
	if ( IsNavigationEnabled() )
	{
		for ( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> WidgetNavigation : WidgetNavigations )
		{
			WidgetNavigation->ForEachWidgetNavigation( Callback );
		}
	}
}

void UEWN_WidgetNavigationSwitcher::InvalidateNavigation()
{
	for ( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> WidgetNavigation : WidgetNavigations )
	{
		WidgetNavigation->InvalidateNavigation();
	}

	ActiveIndex = INDEX_NONE;
}
