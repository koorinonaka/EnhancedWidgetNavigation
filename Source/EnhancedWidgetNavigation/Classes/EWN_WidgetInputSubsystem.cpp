// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetInputSubsystem.h"

#include "Components/Widget.h"
#include "EWN_WidgetInputSettings.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/EWN_Interface_PlayerInputExtension.h"
#include "UObject/ObjectKey.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#endif

class FEWN_WidgetInputProcessor final : public IInputProcessor
{
	friend class UEWN_WidgetInputSubsystem;

public:
	explicit FEWN_WidgetInputProcessor( UEWN_WidgetInputSubsystem& InInputSubsystem ) : InputSubsystem( InInputSubsystem ) {}

protected:
	virtual void Tick( const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor ) override
	{
		// pure virtual
	}

	virtual bool HandleKeyDownEvent( FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent ) override
	{
		if ( const EEWN_WidgetInputMethod InputMethod = GetInputMethod( InKeyEvent.GetKey() );
			 IsRelevantInput( SlateApp, InKeyEvent, InputMethod ) )
		{
			RefreshCurrentInputMethod( InputMethod );
		}

		return false;
	}

	virtual bool HandleAnalogInputEvent( FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent ) override
	{
		if ( const EEWN_WidgetInputMethod InputMethod = GetInputMethod( InAnalogInputEvent.GetKey() );
			 IsRelevantInput( SlateApp, InAnalogInputEvent, InputMethod ) )
		{
			RefreshCurrentInputMethod( InputMethod );
		}

		return false;
	}

	virtual bool HandleMouseMoveEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		if ( const EEWN_WidgetInputMethod InputMethod = GetInputMethod( InPointerEvent );
			 IsRelevantInput( SlateApp, InPointerEvent, InputMethod ) )
		{
			if ( !InPointerEvent.GetCursorDelta().IsNearlyZero() )
			{
				RefreshCurrentInputMethod( InputMethod );
			}
		}

		return false;
	}

	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		if ( const EEWN_WidgetInputMethod InputMethod = GetInputMethod( InPointerEvent );
			 IsRelevantInput( SlateApp, InPointerEvent, InputMethod ) )
		{
			RefreshCurrentInputMethod( InputMethod );
		}

		return false;
	}

	virtual bool HandleMouseButtonDoubleClickEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		if ( const EEWN_WidgetInputMethod InputMethod = GetInputMethod( InPointerEvent );
			 IsRelevantInput( SlateApp, InPointerEvent, InputMethod ) )
		{
			RefreshCurrentInputMethod( InputMethod );
		}

		return false;
	}

private:
	bool IsRelevantInput(
		const FSlateApplication& SlateApp, const FInputEvent& InputEvent, const EEWN_WidgetInputMethod DesiredInputMethod ) const
	{
		if ( SlateApp.IsActive() || SlateApp.GetHandleDeviceInputWhenApplicationNotActive() )
		{
			const ULocalPlayer& LocalPlayer = *InputSubsystem.GetLocalPlayerChecked();
			return LocalPlayer.GetControllerId() == InputEvent.GetUserIndex();
		}
		return false;
	}

	void RefreshCurrentInputMethod( EEWN_WidgetInputMethod InputMethod )
	{
#if WITH_EDITOR && 0
		// Lots of special-case fun for PIE - there are special scenarios wherein we want to ignore the update attempt
		const ULocalPlayer& LocalPlayer = *InputSubsystem.GetLocalPlayerChecked();
		bool bCanApplyInputMethodUpdate = false;
		if ( LocalPlayer.ViewportClient )
		{
			TSharedPtr<FSlateUser> SlateUser = FSlateApplication::Get().GetUser( LocalPlayer.GetControllerId() );
			if ( ensure( SlateUser ) )
			{
				bool bRoutingGamepadToNextPlayer = false;
				if ( !GetDefault<ULevelEditorPlaySettings>()->GetRouteGamepadToSecondWindow( bRoutingGamepadToNextPlayer ) )
				{
					// This looks strange - it's because the getter will set the output param based on the value of the setting,
					// but return whether the setting actually matters. So we're making sure that even if the setting is true,
					// we revert to false when it's irrelevant.
					bRoutingGamepadToNextPlayer = false;
				}

				if ( SlateUser->IsWidgetInFocusPath( LocalPlayer.ViewportClient->GetGameViewportWidget() ) )
				{
					// Our owner's game viewport is in the focus path, which in a PIE scenario means we shouldn't
					// acknowledge gamepad input if it's being routed to another PIE client
					if ( InputMethod != EEWN_WidgetInputMethod::Gamepad || !bRoutingGamepadToNextPlayer )
					{
						bCanApplyInputMethodUpdate = true;
					}
				}
				else if ( InputMethod == EEWN_WidgetInputMethod::Gamepad )
				{
					bCanApplyInputMethodUpdate = bRoutingGamepadToNextPlayer;
				}
			}
		}
		if ( !bCanApplyInputMethodUpdate )
		{
			return;
		}
#endif

		InputSubsystem.SetCurrentInputMethod( InputMethod );
	}

	EEWN_WidgetInputMethod GetInputMethod( const FKey& Key ) const
	{
		if ( Key.IsGamepadKey() )
		{
			return EEWN_WidgetInputMethod::Gamepad;
		}

		return EEWN_WidgetInputMethod::Keyboard;
	}

	EEWN_WidgetInputMethod GetInputMethod( const FPointerEvent& PointerEvent ) const
	{
		if ( PointerEvent.IsTouchEvent() )
		{
			return EEWN_WidgetInputMethod::Touch;
		}

		return EEWN_WidgetInputMethod::Mouse;
	}

private:
	UEWN_WidgetInputSubsystem& InputSubsystem;
};

UEWN_WidgetInputSubsystem* UEWN_WidgetInputSubsystem::Get( UWidget* Widget )
{
	const ULocalPlayer* OwningLP = Widget ? Widget->GetOwningLocalPlayer() : nullptr;
	return OwningLP ? OwningLP->GetSubsystem<ThisClass>() : nullptr;
}

void UEWN_WidgetInputSubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	GetMutableDefault<UEWN_WidgetInputSettings>()->TryLoadObjects();

	Super::Initialize( Collection );

	InputProcessor = MakeShared<FEWN_WidgetInputProcessor>( *this );
	FSlateApplication::Get().RegisterInputPreProcessor( InputProcessor );
}

void UEWN_WidgetInputSubsystem::Deinitialize()
{
	FSlateApplication::Get().UnregisterInputPreProcessor( InputProcessor );
	InputProcessor.Reset();

	Super::Deinitialize();
}

void UEWN_WidgetInputSubsystem::InitPlayerInput( IEWN_Interface_PlayerInputExtension* IPlayerInputExtension )
{
	auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	if ( IPlayerInputExtension )
	{
		FEWN_InputMappingOverrides& IMOverrides =
			IPlayerInputExtension->GetInputMappingOverrides().Emplace( GetTypeHash( FObjectKey( this ) ) );

		auto* WidgetInputSettings = GetDefault<UEWN_WidgetInputSettings>();
		check( WidgetInputSettings );

		IMOverrides.InputMappingContext = WidgetInputSettings->BuildInputMappingContext(
			[&]( FName InputName, UInputAction* IA ) { IMOverrides.InputActions.Emplace( InputName, IA ); } );
		if ( IMOverrides.InputMappingContext )
		{
			EnhancedInputSubsystem->AddMappingContext( IMOverrides.InputMappingContext, 0 );
		}

		EnhancedInputSubsystem->AddMappingContext( WidgetInputSettings->GetOptionalInputMappingContext(), 0 );
	}
#if WITH_EDITOR
	else if ( GIsEditor )
	{
		FMessageLog AssetCheckLog( "AssetCheck" );

		const FText Message(
			NSLOCTEXT( "EWN", "NoSupportEnhancedInput", "PlayerInput does not implement IEWN_Interface_PlayerInputExtension." ) );
		AssetCheckLog.Error( Message );

		AssetCheckLog.Notify( Message, EMessageSeverity::Error, true );
	}
#endif
}

ETriggerEvent UEWN_WidgetInputSubsystem::GetTriggerEvent( UInputAction* IA ) const
{
	const auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	if ( const UEnhancedPlayerInput* EnhancedPI = EnhancedInputSubsystem->GetPlayerInput() )
	{
		if ( const FInputActionInstance* IAInstance = EnhancedPI->FindActionInstanceData( IA ) )
		{
			return IAInstance->GetTriggerEvent();
		}
	}

	return ETriggerEvent::None;
}

ETriggerEvent UEWN_WidgetInputSubsystem::GetTriggerEvent( const UObject* ContextObject, FName InputName ) const
{
	UInputAction* FoundIA = [&]
	{
		const auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check( EnhancedInputSubsystem );

		auto* IPlayerInputExtension = Cast<IEWN_Interface_PlayerInputExtension>( EnhancedInputSubsystem->GetPlayerInput() );
		check( IPlayerInputExtension );

		if ( const FEWN_InputMappingOverrides* IMOverrides =
				 IPlayerInputExtension->GetInputMappingOverrides().Find( GetTypeHash( FObjectKey( ContextObject ) ) ) )
		{
			if ( IMOverrides->InputActions.Contains( InputName ) )
			{
				return IMOverrides->InputActions[InputName];
			}
		}

		const FEWN_InputMappingOverrides* IMOverrides =
			IPlayerInputExtension->GetInputMappingOverrides().Find( GetTypeHash( FObjectKey( this ) ) );
		check( IMOverrides );

		return IMOverrides->InputActions.Contains( InputName ) ? IMOverrides->InputActions[InputName] : nullptr;
	}();
	return FoundIA ? GetTriggerEvent( FoundIA ) : ETriggerEvent::None;
}

bool UEWN_WidgetInputSubsystem::WasJustTriggered( const UObject* ContextObject, FName InputName ) const
{
	return GetTriggerEvent( ContextObject, InputName ) == ETriggerEvent::Triggered;
}

void UEWN_WidgetInputSubsystem::SetInputMappingContext(
	const UObject* ContextObject, const FEWN_WidgetInputMappingContainer& InjectionSettings )
{
	ClearInputMappingContext( ContextObject );

	auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	auto* IPlayerInputExtension = Cast<IEWN_Interface_PlayerInputExtension>( EnhancedInputSubsystem->GetPlayerInput() );
	check( IPlayerInputExtension );

	FEWN_InputMappingOverrides& IMOverrides =
		IPlayerInputExtension->GetInputMappingOverrides().Emplace( GetTypeHash( FObjectKey( ContextObject ) ) );

	IMOverrides.InputMappingContext = GetDefault<UEWN_WidgetInputSettings>()->BuildInputMappingContext(
		InjectionSettings, [&]( FName InputName, UInputAction* IA ) { IMOverrides.InputActions.Emplace( InputName, IA ); } );
	if ( IMOverrides.InputMappingContext )
	{
		EnhancedInputSubsystem->AddMappingContext( IMOverrides.InputMappingContext, 0 );
	}
}

void UEWN_WidgetInputSubsystem::ClearInputMappingContext( const UObject* ContextObject )
{
	auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	auto* IPlayerInputExtension = Cast<IEWN_Interface_PlayerInputExtension>( EnhancedInputSubsystem->GetPlayerInput() );
	check( IPlayerInputExtension );

	if ( const FEWN_InputMappingOverrides* IMOverrides =
			 IPlayerInputExtension->GetInputMappingOverrides().Find( GetTypeHash( FObjectKey( ContextObject ) ) ) )
	{
		EnhancedInputSubsystem->RemoveMappingContext( IMOverrides->InputMappingContext );
	}
}

EEWN_WidgetInputMethod UEWN_WidgetInputSubsystem::GetCurrentInputMethod() const
{
	return CurrentMode;
}

void UEWN_WidgetInputSubsystem::SetCurrentInputMethod( EEWN_WidgetInputMethod NewMode )
{
	if ( CurrentMode != NewMode )
	{
		CurrentMode = NewMode;
		BroadcastInputMethodChanged();
	}
}

void UEWN_WidgetInputSubsystem::BroadcastInputMethodChanged()
{
	if ( const UWorld* World = GetWorld(); World && !World->bIsTearingDown )
	{
		OnInputMethodChangedDelegate.Broadcast( CurrentMode );
	}
}
