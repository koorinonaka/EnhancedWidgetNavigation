// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigationSwitcher.h"

//
#include "Components/Widget.h"

//
#include "EWN_WidgetInputSubsystem.h"
#include "Navigation/CursorHandler/EWN_WidgetNavigationCursorHandler.h"
#include "Navigation/EWN_WidgetNavigation.h"
#include "Navigation/EWN_WidgetNavigationHelper.h"

class FScopedFinalizer
{
private:
	TFunction<void()> Callback;

public:
	explicit FScopedFinalizer( const TFunction<void()> f ) : Callback( f ) {}
	FScopedFinalizer( FScopedFinalizer const& ) = delete;
	void operator=( FScopedFinalizer const& ) = delete;

	~FScopedFinalizer() noexcept( false ) { Callback(); };
};

UEWN_WidgetNavigation* UEWN_WidgetNavigationSwitcher::TickNavigation( float DeltaTime, EEWN_WidgetInputType& OutInputType )
{
	OutInputType = EEWN_WidgetInputType::None;

	auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
	if ( !WidgetInputSubsystem )
	{
		return nullptr;
	}

	switch ( WidgetInputSubsystem->GetCurrentInputMode() )
	{
	case EEWN_WidgetInputMode::Mouse:
	{
		for ( UEWN_WidgetNavigation* WidgetNavigation : WidgetNavigations )
		{
			int32 FoundIndex = WidgetNavigation->FindHoveredIndex();
			if ( FoundIndex != INDEX_NONE )
			{
				WidgetNavigation->UpdateFocusIndex( FoundIndex, true );
			}
		}
	}
	break;
	}

	if ( LastActiveNavigation.IsValid() )
	{
		check( WidgetNavigations.Contains( LastActiveNavigation.Get() ) );

		EEWN_WidgetInputType InputResult = LastActiveNavigation->TickNavigation( DeltaTime );
		if ( InputResult != EEWN_WidgetInputType::None )
		{
			OutInputType = InputResult;
			return LastActiveNavigation.Get();
		}
	}

	return nullptr;
}

bool UEWN_WidgetNavigationSwitcher::IsActive( UEWN_WidgetNavigation* WidgetNavigation ) const
{
	return WidgetNavigation && WidgetNavigation == LastActiveNavigation;
}

int32 UEWN_WidgetNavigationSwitcher::GetNavigationCount() const
{
	return WidgetNavigations.Num();
}

UEWN_WidgetNavigation* UEWN_WidgetNavigationSwitcher::GetNavigationAt( int32 Index ) const
{
	return Index < WidgetNavigations.Num() ? WidgetNavigations[Index] : nullptr;
}

TArray<UEWN_WidgetNavigation*> UEWN_WidgetNavigationSwitcher::GetAllNavigations() const
{
	return WidgetNavigations;
}

void UEWN_WidgetNavigationSwitcher::Register( UEWN_WidgetNavigation* WidgetNavigation, bool bConnectNavigation )
{
	if ( ensureAlways( WidgetNavigation ) )
	{
		WidgetNavigation->OnFocusUpdatedDelegate.AddDynamic( this, &ThisClass::OnNavigationFocusUpdated );
		WidgetNavigation->SetMoveFocusFallback(
			UEWN_WidgetNavigation::FMoveFocusDelegate::CreateUObject( this, &ThisClass::MoveFocusFallback ) );
		WidgetNavigations.Emplace( WidgetNavigation );

		FWidgetNavigation& NavigationInfo = WidgetNavigationKeys.Emplace( WidgetNavigation );
		NavigationInfo.bConnectNavigation = bConnectNavigation;
	}
}

void UEWN_WidgetNavigationSwitcher::Unregister( UEWN_WidgetNavigation* WidgetNavigation )
{
	if ( ensureAlways( WidgetNavigation ) )
	{
		WidgetNavigation->OnFocusUpdatedDelegate.RemoveDynamic( this, &ThisClass::OnNavigationFocusUpdated );
		WidgetNavigation->SetMoveFocusFallback( UEWN_WidgetNavigation::FMoveFocusDelegate() );
		WidgetNavigations.Remove( WidgetNavigation );

		WidgetNavigationKeys.Remove( WidgetNavigation );
	}
}

void UEWN_WidgetNavigationSwitcher::AddRoute(
	EEWN_WidgetCursor WidgetCursor, UEWN_WidgetNavigation* Source, UEWN_WidgetNavigation* Destination )
{
	if ( ensureAlways( Source && Destination ) )
	{
		FWidgetNavigationOverride Key;
		Key.Navigation = Source;
		Key.WidgetCursor = WidgetCursor;

		if ( WidgetNavigationOverrides.Num( Key ) == 0 )
		{
			Source->SetMoveFocusOverride(
				UEWN_WidgetNavigation::FMoveFocusDelegate::CreateUObject( this, &ThisClass::MoveFocusOverride ) );
		}

		WidgetNavigationOverrides.Emplace( Key, Destination );
	}
}

void UEWN_WidgetNavigationSwitcher::RemoveRoute(
	EEWN_WidgetCursor WidgetCursor, UEWN_WidgetNavigation* Source, UEWN_WidgetNavigation* Destination )
{
	if ( ensureAlways( Source && Destination ) )
	{
		FWidgetNavigationOverride Key;
		Key.Navigation = Source;
		Key.WidgetCursor = WidgetCursor;

		WidgetNavigationOverrides.Remove( Key, Destination );

		if ( WidgetNavigationOverrides.Num( Key ) == 0 )
		{
			Source->SetMoveFocusOverride( UEWN_WidgetNavigation::FMoveFocusDelegate() );
		}
	}
}

bool UEWN_WidgetNavigationSwitcher::MoveFocusOverride(
	UEWN_WidgetNavigation* Navigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	using namespace EWN::WidgetNavigationHelper;

	FWidgetNavigationOverride Key;
	Key.Navigation = Navigation;
	Key.WidgetCursor = WidgetCursor;

	if ( WidgetNavigationOverrides.Contains( Key ) )
	{
		check( Navigation->CursorHandler );

		// temporarily turn off navigation
		bool bLoop = Navigation->bLoopNavigation;
		Navigation->bLoopNavigation = false;
		FScopedFinalizer F( [&] { Navigation->bLoopNavigation = bLoop; } );

		int32 NewIndex = Navigation->CursorHandler->GetNextIndex( Navigation->FocusIndex, WidgetCursor );
		if ( NewIndex != Navigation->FocusIndex )
		{
			// default navigation worked, so use it
			return false;
		}

		UWidget* CurrentWidget = Navigation->GetChildAt( Navigation->GetFocusIndex() );
		check( CurrentWidget );

		TArray<TWeakObjectPtr<UEWN_WidgetNavigation>> Destinations;
		WidgetNavigationOverrides.MultiFind( Key, Destinations );

		TMap<UWidget*, FWidgetWithNavigation> WidgetsWithNavigation;
		for ( TWeakObjectPtr<UEWN_WidgetNavigation> Destination : Destinations )
		{
			if ( !Destination.IsValid() )
			{
				continue;
			}

			Destination->ForEachFocusable(
				[&]( int32 i, UWidget* ChildWidget )
				{
					FWidgetWithNavigation& Info = WidgetsWithNavigation.Emplace( ChildWidget );
					Info.Navigation = Destination.Get();
					Info.Index = i;
					Info.Position = GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
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
	UEWN_WidgetNavigation* Navigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	using namespace EWN::WidgetNavigationHelper;

	UWidget* CurrentWidget = Navigation->GetChildAt( Navigation->GetFocusIndex() );
	check( CurrentWidget );

	TMap<UWidget*, FWidgetWithNavigation> WidgetsWithNavigation;
	for ( const TPair<TObjectKey<UEWN_WidgetNavigation>, FWidgetNavigation>& KVP : WidgetNavigationKeys )
	{
		UEWN_WidgetNavigation* WidgetNavigation = KVP.Key.ResolveObjectPtr();
		if ( !WidgetNavigation || WidgetNavigation == Navigation || !KVP.Value.bConnectNavigation )
		{
			continue;
		}

		WidgetNavigation->ForEachFocusable(
			[&]( int32 i, UWidget* ChildWidget )
			{
				FWidgetWithNavigation& Info = WidgetsWithNavigation.Emplace( ChildWidget );
				Info.Navigation = WidgetNavigation;
				Info.Index = i;
				Info.Position = GetCursorPosition( ChildWidget->GetCachedGeometry(), EEWN_WidgetCursor::None );
			} );
	}

	// fallback if MoveFocus fails
	if ( UWidget* NearestWidget = FindFocusToNearest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
	{
		const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[NearestWidget];
		WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
		return true;
	}
	else if ( bLoopNavigation )
	{
		if ( UWidget* FarthestWidget = FindFocusToFarthest( CurrentWidget, WidgetCursor, WidgetsWithNavigation ) )
		{
			const FWidgetWithNavigation& WidgetInfo = WidgetsWithNavigation[FarthestWidget];
			WidgetInfo.Navigation->UpdateFocusIndex( WidgetInfo.Index, bFromOperation );
			return true;
		}
	}

	return false;
}

void UEWN_WidgetNavigationSwitcher::OnNavigationFocusUpdated(
	UEWN_WidgetNavigation* Navigation, int32 OldIndex, int32 NewIndex, bool bFromOperation )
{
	if ( NewIndex != INDEX_NONE && LastActiveNavigation != Navigation )
	{
		LastActiveNavigation = Navigation;

		for ( UEWN_WidgetNavigation* WidgetNavigation : WidgetNavigations )
		{
			if ( WidgetNavigation && WidgetNavigation != LastActiveNavigation )
			{
				// WidgetSwitcher has only one focus.
				WidgetNavigation->UpdateFocusIndex( INDEX_NONE, false );
			}
		}
	}
}
