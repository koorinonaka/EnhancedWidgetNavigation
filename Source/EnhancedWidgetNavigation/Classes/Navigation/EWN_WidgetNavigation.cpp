// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetNavigation.h"

//
#include "Components/PanelWidget.h"
#include "EnhancedInputSubsystems.h"

//
#include "EWN_WidgetInputSettings.h"
#include "EWN_WidgetInputSubsystem.h"
#include "Navigation/CursorHandler/EWN_WidgetNavigationCursorHandler.h"
#include "Navigation/EWN_WidgetNavigationSubsystem.h"

namespace EWNUtil
{
UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem( UWidget* Widget )
{
	ULocalPlayer* LP = Widget ? Widget->GetOwningLocalPlayer() : nullptr;
	return LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
};
}	 // namespace EWNUtil

void UEWN_WidgetNavigation::PostInitProperties()
{
	Super::PostInitProperties();

	CursorHandler = FEWN_WidgetNavigationCursorFactory::CreateHandler( this );
}

void UEWN_WidgetNavigation::BeginDestroy()
{
	if ( IMC_Navigation )
	{
		UE_LOG( LogTemp, Warning, TEXT( "you forgot to call ClearInputMappingContext!" ) );
	}

	Super::BeginDestroy();
}

void UEWN_WidgetNavigation::SetInputMappingContext( const FEWN_WidgetInputMappingContainer& InjectionSettings )
{
	ClearInputMappingContext();

	IMC_Navigation = GetDefault<UEWN_WidgetInputSettings>()->BuildInputMappingContext( InjectionSettings,
		[this]( EEWN_WidgetInputType InputType, UInputAction* IA ) { IA_NavigationActions.Emplace( InputType, IA ); } );
	if ( IMC_Navigation )
	{
		if ( UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =
				 EWNUtil::GetEnhancedInputSubsystem( GetTypedOuter<UWidget>() ) )
		{
			EnhancedInputSubsystem->AddMappingContext( IMC_Navigation, 0 );
		}
	}
}

void UEWN_WidgetNavigation::ClearInputMappingContext()
{
	if ( IMC_Navigation )
	{
		if ( UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =
				 EWNUtil::GetEnhancedInputSubsystem( GetTypedOuter<UWidget>() ) )
		{
			EnhancedInputSubsystem->RemoveMappingContext( IMC_Navigation );
		}

		IMC_Navigation = nullptr;
		IA_NavigationActions.Empty();
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

	auto WasJustTriggeredWithCancel = [&]( EEWN_WidgetInputType TriggerInput, EEWN_WidgetInputType CancelInput )
	{
		ETriggerEvent CancelEvent = GetTriggerEvent( CancelInput );
		bool bCancelled = CancelEvent == ETriggerEvent::Triggered || CancelEvent == ETriggerEvent::Ongoing;
		return !bCancelled && WasJustTriggered( TriggerInput );
	};

	if ( WasJustTriggeredWithCancel( EEWN_WidgetInputType::Up, EEWN_WidgetInputType::Down ) )
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
	else
	{
		auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
		if ( !WidgetInputSubsystem )
		{
			return EEWN_WidgetInputType::None;
		}

		int32 NewIndex = [&]( int32 CurrentIndex )
		{
			int32 ResultIndex = CurrentIndex;

			bool bCheckHovered = bMouseEnabled && WidgetInputSubsystem->GetCurrentInputMode() == EEWN_WidgetInputMode::Mouse;
			if ( bCheckHovered )
			{
				bool bValidHovered = [&]
				{
					if ( CurrentIndex != INDEX_NONE )
					{
						UWidget* ChildWidget = GetChildAt( CurrentIndex );
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
		if ( NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, true );
		}

		if ( FocusIndex != INDEX_NONE )
		{
			UWidget* ChildWidget = GetChildAt( FocusIndex );
			if ( IEWN_Interface_WidgetNavigationChild::IsNavigationAcceptable( ChildWidget ) )
			{
				if ( ( WasJustTriggered( EEWN_WidgetInputType::Accept ) ||
						 IEWN_Interface_WidgetNavigationChild::WasJustNavigationClicked( ChildWidget ) ) )
				{
					IEWN_Interface_WidgetNavigationChild::Execute_K2_SetNavigationAccepted( ChildWidget, true );
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
	auto* PanelWidget = GetTypedOuter<UPanelWidget>();
	return ensure( PanelWidget ) ? PanelWidget->GetChildAt( Index ) : nullptr;
}

void UEWN_WidgetNavigation::ForEachFocusableIndex( const TFunctionRef<void( int32 )> Callback )
{
	auto* PanelWidget = GetTypedOuter<UPanelWidget>();
	if ( ensure( PanelWidget ) )
	{
		int32 ChildrenCount = PanelWidget->GetChildrenCount();
		for ( int32 i = 0; i < ChildrenCount; ++i )
		{
			Callback( i );
		}
	}
}

int32 UEWN_WidgetNavigation::FindHoveredIndex() const
{
	auto* PanelWidget = GetTypedOuter<UPanelWidget>();
	if ( ensure( PanelWidget ) )
	{
		int32 ChildrenCount = PanelWidget->GetChildrenCount();
		for ( int32 i = 0; i < ChildrenCount; ++i )
		{
			UWidget* ChildWidget = PanelWidget->GetChildAt( i );
			if ( !ChildWidget->IsHovered() )
			{
				continue;
			}

			if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( ChildWidget ) )
			{
				return i;
			}
		}
	}

	return INDEX_NONE;
}

ETriggerEvent UEWN_WidgetNavigation::GetTriggerEvent( EEWN_WidgetInputType InputType ) const
{
	auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
	if ( !WidgetInputSubsystem )
	{
		return ETriggerEvent::None;
	}

	if ( IA_NavigationActions.Contains( InputType ) )
	{
		UInputAction* IA = IA_NavigationActions[InputType];
		return WidgetInputSubsystem->GetTriggerEvent( IA );
	}
	else
	{
		// 上書きがなければCommonInputを使う
		return WidgetInputSubsystem->GetTriggerEvent( InputType );
	}

	return ETriggerEvent::None;
}

bool UEWN_WidgetNavigation::WasJustTriggered( EEWN_WidgetInputType InputType ) const
{
	return GetTriggerEvent( InputType ) == ETriggerEvent::Triggered;
}

bool UEWN_WidgetNavigation::MoveFocus( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	if ( CursorHandler )
	{
		if ( OnMoveFocusOverride.IsBound() && OnMoveFocusOverride.Execute( this, WidgetCursor, bFromOperation ) )
		{
			return true;
		}

		int32 NewIndex = CursorHandler->GetNextIndex( FocusIndex, WidgetCursor );
		if ( NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, bFromOperation );
			return true;
		}
		else if ( OnMoveFocusFallback.IsBound() )
		{
			return OnMoveFocusFallback.Execute( this, WidgetCursor, bFromOperation );
		}
	}
	return false;
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
		int32 NewIndex = CursorHandler->GetForwardIndex( FocusIndex );
		if ( NewIndex != FocusIndex )
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
		int32 NewIndex = CursorHandler->GetBackwardIndex( FocusIndex );
		if ( NewIndex != FocusIndex )
		{
			UpdateFocusIndex( NewIndex, bFromOperation );
			return true;
		}
	}
	return false;
}

void UEWN_WidgetNavigation::UpdateFocusIndex( int32 NewIndex, bool bFromOperation )
{
	auto* PanelWidget = GetTypedOuter<UPanelWidget>();
	if ( ensure( PanelWidget ) )
	{
		UWidget* FocusWidget = PanelWidget->GetChildAt( NewIndex );
		if ( IEWN_Interface_WidgetNavigationChild::ImplementsInterface( FocusWidget ) )
		{
			check( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( FocusWidget ) );
		}

		int32 OldIndex = FocusIndex;

		int32 ChildrenCount = PanelWidget->GetChildrenCount();
		for ( int32 i = 0; i < ChildrenCount; ++i )
		{
			UWidget* ChildWidget = PanelWidget->GetChildAt( i );
			if ( IEWN_Interface_WidgetNavigationChild::ImplementsInterface( ChildWidget ) )
			{
				bool bShouldUpdate = OldIndex == INDEX_NONE || NewIndex == INDEX_NONE;
				bShouldUpdate |= ( OldIndex != NewIndex && ( OldIndex == i || NewIndex == i ) );	// 更新があった場合だけ通知
				if ( bShouldUpdate )
				{
					IEWN_Interface_WidgetNavigationChild::Execute_K2_SetNavigationFocused(
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

void UEWN_WidgetNavigation::ResetNavigation( bool bResetIndex )
{
	auto* OuterWidget = GetTypedOuter<UWidget>();
	if ( OuterWidget && OuterWidget->IsDesignTime() )
	{
		UpdateFocusIndex( 0, false );
		return;
	}

	auto* WidgetInputSubsystem = UEWN_WidgetInputSubsystem::Get( GetTypedOuter<UWidget>() );
	if ( !WidgetInputSubsystem )
	{
		return;
	}

	bool bMouseOperation = bMouseEnabled && WidgetInputSubsystem->GetCurrentInputMode() == EEWN_WidgetInputMode::Mouse;
	if ( bMouseOperation )
	{
		UpdateFocusIndex( FindHoveredIndex(), false );
	}
	else
	{
		int32 NewIndex = !bResetIndex ? FocusIndex : INDEX_NONE;

		if ( IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( GetChildAt( NewIndex ) ) )
		{
			UpdateFocusIndex( NewIndex, false );
		}
		else
		{
			bool bFlag = bLoopNavigation;
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
	OnFocusUpdatedDelegate.Broadcast( this, OldIndex, NewIndex, bFromOperation );
}

void UEWN_WidgetNavigation::NotifyFocusAccepted( int32 Index )
{
	OnFocusAcceptedDelegate.Broadcast( this, Index );
}
