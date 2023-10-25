// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigation.h"

#include "Components/PanelWidget.h"
#include "EWN_WidgetInputSettings.h"
#include "EWN_WidgetInputSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"
#include "Navigation/CursorHandler/EWN_WidgetNavigationCursorHandler.h"
#include "Navigation/EWN_WidgetNavigationHelper.h"
#include "Navigation/EWN_WidgetNavigationSubsystem.h"

namespace EWN::Util
{
UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem( UWidget* Widget )
{
	const ULocalPlayer* LP = Widget ? Widget->GetOwningLocalPlayer() : nullptr;
	return LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
};
}	 // namespace EWN::Util

class FScopedFinalizer
{
	TFunction<void()> Callback;

public:
	explicit FScopedFinalizer( const TFunction<void()>& F ) : Callback( F ) {}
	FScopedFinalizer( const FScopedFinalizer& ) = delete;
	void operator=( const FScopedFinalizer& ) = delete;

	~FScopedFinalizer() noexcept( false ) { Callback(); };
};

void UEWN_WidgetNavigation::PostInitProperties()
{
	Super::PostInitProperties();

	CursorHandler = EWN::WidgetNavigation::FCursorFactory::CreateHandler( this );
}

void UEWN_WidgetNavigation::SetInputMappingContext( const FEWN_WidgetInputMappingContainer& InjectionSettings )
{
	if ( auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() ) )
	{
		WidgetInputSubsystem->SetInputMappingContext( this, InjectionSettings );
	}
}

void UEWN_WidgetNavigation::ClearInputMappingContext()
{
	if ( auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() ) )
	{
		WidgetInputSubsystem->ClearInputMappingContext( this );
	}
}

EEWN_WidgetInputType UEWN_WidgetNavigation::TickNavigation( float DeltaTime )
{
	auto* WidgetNavigationSubsystem = UEWN_WidgetNavigationSubsystem::Get( GetTypedOuter<UWidget>() );
	if ( !ensureMsgf( WidgetNavigationSubsystem, TEXT( "failed to get subsystem, check if outer widget has owning player." ) ) )
	{
		return EEWN_WidgetInputType::None;
	}

	WidgetNavigationSubsystem->MarkOnThisFrame( this );

	if ( auto WasJustTriggeredWithCancel =
			 [&]( EEWN_WidgetInputType TriggerInput, EEWN_WidgetInputType CancelInput )
		 {
			 const ETriggerEvent CancelEvent = GetTriggerEvent( CancelInput );
			 const bool bCancelled = CancelEvent == ETriggerEvent::Triggered || CancelEvent == ETriggerEvent::Ongoing;
			 return !bCancelled && WasJustTriggered( TriggerInput );
		 };
		 WasJustTriggeredWithCancel( EEWN_WidgetInputType::Up, EEWN_WidgetInputType::Down ) )
	{
		if ( RestoreFocus( true ) || MoveFocus( EEWN_WidgetCursor::Up, true ) )
		{
			return EEWN_WidgetInputType::Up;
		}
	}
	else if ( WasJustTriggeredWithCancel( EEWN_WidgetInputType::Down, EEWN_WidgetInputType::Up ) )
	{
		if ( RestoreFocus( true ) || MoveFocus( EEWN_WidgetCursor::Down, true ) )
		{
			return EEWN_WidgetInputType::Down;
		}
	}
	else if ( WasJustTriggeredWithCancel( EEWN_WidgetInputType::Left, EEWN_WidgetInputType::Right ) )
	{
		if ( RestoreFocus( true ) || MoveFocus( EEWN_WidgetCursor::Left, true ) )
		{
			return EEWN_WidgetInputType::Left;
		}
	}
	else if ( WasJustTriggeredWithCancel( EEWN_WidgetInputType::Right, EEWN_WidgetInputType::Left ) )
	{
		if ( RestoreFocus( true ) || MoveFocus( EEWN_WidgetCursor::Right, true ) )
		{
			return EEWN_WidgetInputType::Right;
		}
	}
	else if ( WasJustTriggeredWithCancel( EEWN_WidgetInputType::Prev, EEWN_WidgetInputType::Next ) )
	{
		if ( RestoreFocus( true ) || FocusDecrement( true ) )
		{
			return EEWN_WidgetInputType::Prev;
		}
	}
	else if ( WasJustTriggeredWithCancel( EEWN_WidgetInputType::Next, EEWN_WidgetInputType::Prev ) )
	{
		if ( RestoreFocus( true ) || FocusIncrement( true ) )
		{
			return EEWN_WidgetInputType::Next;
		}
	}
	else if ( WasJustTriggered( EEWN_WidgetInputType::Back ) )
	{
		return EEWN_WidgetInputType::Back;
	}
	else
	{
		const auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
		if ( !WidgetInputSubsystem )
		{
			return EEWN_WidgetInputType::None;
		}

		if ( const int32 NewIndex =
				 [&]( int32 CurrentIndex )
			 {
				 int32 ResultIndex = CurrentIndex;

				 bool bCheckHovered =
					 bMouseEnabled && WidgetInputSubsystem->GetCurrentInputMethod() == EEWN_WidgetInputMethod::Mouse;
				 if ( bCheckHovered )
				 {
					 const bool bValidHovered = [&]
					 {
						 if ( CurrentIndex != INDEX_NONE )
						 {
							 const UWidget* ChildWidget = GetChildAt( CurrentIndex );
							 return ChildWidget && ChildWidget->IsHovered();
						 }
						 return false;
					 }();
					 if ( !bValidHovered )
					 {
						 // Focusが外れた
						 ResultIndex = FindHoveredIndex();
					 }
				 }

				 return ResultIndex;
			 }( FocusIndex );
			 NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, true );
		}

		if ( FocusIndex != INDEX_NONE )
		{
			if ( UWidget* ChildWidget = GetChildAt( FocusIndex );
				 IEWN_Interface_WidgetNavigationChild::IsNavigationAcceptable( ChildWidget ) )
			{
				if ( ( WasJustTriggered( EEWN_WidgetInputType::Accept ) ||
						 IEWN_Interface_WidgetNavigationChild::WasJustNavigationClicked( ChildWidget ) ) )
				{
					IEWN_Interface_WidgetNavigationChild::Execute_K2_NavigationAccepted( ChildWidget, true );
					NotifyFocusAccepted( FocusIndex );
					return EEWN_WidgetInputType::Accept;
				}
			}
		}
	}

	return EEWN_WidgetInputType::None;
}

UWidget* UEWN_WidgetNavigation::GetChildAt( int32 Index ) const
{
	const auto* PanelWidget = GetTypedOuter<UPanelWidget>();
	return ensure( PanelWidget ) ? PanelWidget->GetChildAt( Index ) : nullptr;
}

int32 UEWN_WidgetNavigation::GetChildIndex( UWidget* Widget ) const
{
	const auto* PanelWidget = GetTypedOuter<UPanelWidget>();
	return ensure( PanelWidget ) ? PanelWidget->GetChildIndex( Widget ) : INDEX_NONE;
}

void UEWN_WidgetNavigation::ForEachWidgetNavigation( const TFunctionRef<void( UEWN_WidgetNavigation* )> Callback )
{
	if ( IsNavigationEnabled() )
	{
		Callback( this );
	}
}

int32 UEWN_WidgetNavigation::FindHoveredIndex() const
{
	return EWN::WidgetNavigation::FHelper::FindPanelIndex(
		GetTypedOuter<UPanelWidget>(), [&]( int32 i, UWidget* ChildWidget ) {	 //
			return ChildWidget->IsHovered() && IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( ChildWidget );
		} );
}

void UEWN_WidgetNavigation::ForEachFocusable( const TFunctionRef<void( int32, UWidget* )> Callback ) const
{
	EWN::WidgetNavigation::FHelper::ForEachPanelChildren( GetTypedOuter<UPanelWidget>(),
		[&]( int32 i, UWidget* ChildWidget )
		{
			if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( ChildWidget ) )
			{
				Callback( i, ChildWidget );
			}
		} );
}

ETriggerEvent UEWN_WidgetNavigation::GetTriggerEvent( EEWN_WidgetInputType InputType ) const
{
	const auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
	return WidgetInputSubsystem ? WidgetInputSubsystem->GetTriggerEvent( this, *EWN::Enum::GetValueAsString( InputType ) )
								: ETriggerEvent::None;
}

bool UEWN_WidgetNavigation::WasJustTriggered( EEWN_WidgetInputType InputType ) const
{
	return GetTriggerEvent( InputType ) == ETriggerEvent::Triggered;
}

bool UEWN_WidgetNavigation::MoveFocus( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	if ( CursorHandler )
	{
		if ( TryMoveFocusOverride( WidgetCursor, bFromOperation ) )
		{
			return true;
		}

		if ( const int32 NewIndex = CursorHandler->GetNextIndex( FocusIndex, WidgetCursor ); NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, bFromOperation );
			return true;
		}

		return !bIndependentNavigation && TryMoveFocusFallback( WidgetCursor, bFromOperation );
	}
	return false;
}

bool UEWN_WidgetNavigation::TestFocus( EEWN_WidgetCursor WidgetCursor ) const
{
	check( CursorHandler );

	ThisClass* MutableThis = const_cast<ThisClass*>( this );

	// temporarily turn off navigation
	const bool bLoop = MutableThis->bLoopNavigation;
	MutableThis->bLoopNavigation = false;
	FScopedFinalizer F( [&] { MutableThis->bLoopNavigation = bLoop; } );

	return CursorHandler->GetNextIndex( FocusIndex, WidgetCursor ) != FocusIndex;
}

bool UEWN_WidgetNavigation::RestoreFocus( bool bFromOperation )
{
	if ( FocusIndex == INDEX_NONE && LastValidFocusIndex != INDEX_NONE )
	{
		UpdateFocusIndex( LastValidFocusIndex, bFromOperation );
		return true;
	}
	return false;
}

bool UEWN_WidgetNavigation::FocusIncrement( bool bFromOperation )
{
	if ( CursorHandler )
	{
		if ( const int32 NewIndex = CursorHandler->GetForwardIndex( FocusIndex ); NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, bFromOperation );
			return true;
		}
	}
	return false;
}

bool UEWN_WidgetNavigation::FocusDecrement( bool bFromOperation )
{
	if ( CursorHandler )
	{
		if ( const int32 NewIndex = CursorHandler->GetBackwardIndex( FocusIndex ); NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, bFromOperation );
			return true;
		}
	}
	return false;
}

void UEWN_WidgetNavigation::UpdateFocusIndex( int32 NewIndex, bool bFromOperation )
{
	if ( const auto* PanelWidget = GetTypedOuter<UPanelWidget>(); ensure( PanelWidget ) )
	{
		if ( const UWidget* FocusWidget = PanelWidget->GetChildAt( NewIndex );
			 IEWN_Interface_WidgetNavigationChild::ImplementsInterface( FocusWidget ) )
		{
			check( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( FocusWidget ) );
		}

		const int32 OldIndex = FocusIndex;

		const int32 ChildrenCount = PanelWidget->GetChildrenCount();
		for ( int32 i = 0; i < ChildrenCount; ++i )
		{
			if ( UWidget* ChildWidget = PanelWidget->GetChildAt( i );
				 IEWN_Interface_WidgetNavigationChild::ImplementsInterface( ChildWidget ) )
			{
				bool bShouldUpdate = OldIndex == INDEX_NONE || NewIndex == INDEX_NONE;
				bShouldUpdate |= ( OldIndex != NewIndex && ( OldIndex == i || NewIndex == i ) );	// 更新があった場合だけ通知
				if ( bShouldUpdate )
				{
					IEWN_Interface_WidgetNavigationChild::Execute_K2_NavigationFocused(
						ChildWidget, i == NewIndex, bFromOperation );
				}
			}
		}

		FocusIndex = NewIndex;

		if ( FocusIndex != INDEX_NONE )
		{
			LastValidFocusIndex = FocusIndex;
		}

		if ( OldIndex != NewIndex )
		{
			NotifyFocusUpdated( OldIndex, NewIndex, bFromOperation );
		}
	}
}

UWidget* UEWN_WidgetNavigation::GetCurrentWidget() const
{
	return GetChildAt( FocusIndex );
}

void UEWN_WidgetNavigation::MoveWithMouseCursor()
{
	if ( const int32 FoundIndex = FindHoveredIndex(); FoundIndex != INDEX_NONE )
	{
		UpdateFocusIndex( FoundIndex, true );
	}
}

void UEWN_WidgetNavigation::InvalidateNavigation()
{
	UpdateFocusIndex( INDEX_NONE, false );
}

void UEWN_WidgetNavigation::ResetNavigation( bool bResetIndex )
{
	if ( const auto* OuterWidget = GetTypedOuter<UWidget>(); OuterWidget && OuterWidget->IsDesignTime() )
	{
		UpdateFocusIndex( 0, false );
		return;
	}

	const auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
	if ( !WidgetInputSubsystem )
	{
		return;
	}

	bool bMouseOperation = bMouseEnabled && WidgetInputSubsystem->GetCurrentInputMethod() == EEWN_WidgetInputMethod::Mouse;
	if ( bMouseOperation )
	{
		UpdateFocusIndex( FindHoveredIndex(), false );
	}
	else
	{
		if ( const int32 NewIndex = !bResetIndex ? FocusIndex : INDEX_NONE;
			 IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( GetChildAt( NewIndex ) ) )
		{
			UpdateFocusIndex( NewIndex, false );
		}
		else
		{
			const bool bFlag = bLoopNavigation;
			bLoopNavigation = false;
			{
				// カーソルが無効だった場合は前後を探す
				FocusIndex = NewIndex;
				if ( !FocusIncrement( false ) && !FocusDecrement( false ) )
				{
					UpdateFocusIndex( INDEX_NONE, false );
				}
			}
			bLoopNavigation = bFlag;
		}
	}
}

void UEWN_WidgetNavigation::NotifyFocusUpdated( int32 OldIndex, int32 NewIndex, bool bFromOperation )
{
	FocusUpdatedDelegate.Broadcast( this, OldIndex, NewIndex, bFromOperation );
	NavigationUpdatedDelegate.Broadcast( this, OldIndex, NewIndex, bFromOperation );
}

void UEWN_WidgetNavigation::NotifyFocusAccepted( int32 Index )
{
	FocusAcceptedDelegate.Broadcast( this, Index );
}
