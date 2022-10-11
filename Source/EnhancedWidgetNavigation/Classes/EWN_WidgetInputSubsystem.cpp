// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetInputSubsystem.h"

//
#include "Components/Widget.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"

//
#include "EWN_WidgetInputSettings.h"
#include "Interfaces/EWN_Interface_LocalPlayerExtension.h"

#if WITH_EDITOR
#include "IMessageLogListing.h"
#include "MessageLogModule.h"
#endif

class FEWN_WidgetInputProcessor : public IInputProcessor
{
	friend class UEWN_WidgetInputSubsystem;

public:
	FEWN_WidgetInputProcessor( UEWN_WidgetInputSubsystem& InInputSubsystem ) : InputSubsystem( InInputSubsystem ) {}

protected:
	virtual void Tick( const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor ) override
	{
		// pure virtual
	}

	virtual bool HandleKeyDownEvent( FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent ) override
	{
		const EEWN_WidgetInputMode InputMode = GetInputMode( InKeyEvent.GetKey() );
		if ( IsRelevantInput( SlateApp, InKeyEvent, InputMode ) )
		{
			RefreshCurrentInputMethod( InputMode );
		}

		return false;
	}

	virtual bool HandleAnalogInputEvent( FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent ) override
	{
		const EEWN_WidgetInputMode InputMode = GetInputMode( InAnalogInputEvent.GetKey() );
		if ( IsRelevantInput( SlateApp, InAnalogInputEvent, InputMode ) )
		{
			RefreshCurrentInputMethod( InputMode );
		}

		return false;
	}

	virtual bool HandleMouseMoveEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		const EEWN_WidgetInputMode InputMode = GetInputMode( InPointerEvent );
		if ( IsRelevantInput( SlateApp, InPointerEvent, InputMode ) )
		{
			if ( !InPointerEvent.GetCursorDelta().IsNearlyZero() )
			{
				RefreshCurrentInputMethod( InputMode );
			}
		}

		return false;
	}

	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		const EEWN_WidgetInputMode InputMode = GetInputMode( InPointerEvent );
		if ( IsRelevantInput( SlateApp, InPointerEvent, InputMode ) )
		{
			RefreshCurrentInputMethod( InputMode );
		}

		return false;
	}

	virtual bool HandleMouseButtonDoubleClickEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		const EEWN_WidgetInputMode InputMode = GetInputMode( InPointerEvent );
		if ( IsRelevantInput( SlateApp, InPointerEvent, InputMode ) )
		{
			RefreshCurrentInputMethod( InputMode );
		}

		return false;
	}

private:
	bool IsRelevantInput( FSlateApplication& SlateApp, const FInputEvent& InputEvent, const EEWN_WidgetInputMode DesiredInputMode )
	{
		if ( SlateApp.IsActive() || SlateApp.GetHandleDeviceInputWhenApplicationNotActive() )
		{
			const ULocalPlayer& LocalPlayer = *InputSubsystem.GetLocalPlayerChecked();
			return LocalPlayer.GetControllerId() == InputEvent.GetUserIndex();
		}
		return false;
	}

	void RefreshCurrentInputMethod( EEWN_WidgetInputMode InputMethod )
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
					if ( InputMethod != EEWN_WidgetInputMode::Gamepad || !bRoutingGamepadToNextPlayer )
					{
						bCanApplyInputMethodUpdate = true;
					}
				}
				else if ( InputMethod == EEWN_WidgetInputMode::Gamepad )
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

		InputSubsystem.SetCurrentInputMode( InputMethod );
	}

	EEWN_WidgetInputMode GetInputMode( const FKey& Key )
	{
		if ( Key.IsGamepadKey() )
		{
			return EEWN_WidgetInputMode::Gamepad;
		}

		return EEWN_WidgetInputMode::Keyboard;
	}

	EEWN_WidgetInputMode GetInputMode( const FPointerEvent& PointerEvent )
	{
		if ( PointerEvent.IsTouchEvent() )
		{
			return EEWN_WidgetInputMode::Touch;
		}

		return EEWN_WidgetInputMode::Mouse;
	}

private:
	UEWN_WidgetInputSubsystem& InputSubsystem;
};

UEWN_WidgetInputSubsystem* UEWN_WidgetInputSubsystem::Get( UWidget* Widget )
{
	ULocalPlayer* OwningLP = Widget ? Widget->GetOwningLocalPlayer() : nullptr;
	return OwningLP ? OwningLP->GetSubsystem<ThisClass>() : nullptr;
}

void UEWN_WidgetInputSubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	if ( auto* ILPExtension = GetLocalPlayer<IEWN_Interface_LocalPlayerExtension>() )
	{
		ILPExtension->SpawnPlayActorDelegate.AddUObject( this, &ThisClass::InitOnSpawnPlayActor );
	}
#if WITH_EDITOR
	else if ( GIsEditor )
	{
		FMessageLog AssetCheckLog( "AssetCheck" );

		FText Message( NSLOCTEXT(
			"EWN", "LocalPlayerNotImplementsInterface", "LocalPlayer does not implement IEWN_Interface_LocalPlayerExtension." ) );
		AssetCheckLog.Error( Message );

		AssetCheckLog.Notify( Message, EMessageSeverity::Error, true );
	}
#endif

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

void UEWN_WidgetInputSubsystem::InitOnSpawnPlayActor( APlayerController* PlayActor )
{
	IMC_CommonInput = nullptr;
	IA_CommonInputActions.Empty();

	auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	if ( UEnhancedPlayerInput* EnhancedPI = EnhancedInputSubsystem->GetPlayerInput() )
	{
		auto* WidgetInputSettings = GetDefault<UEWN_WidgetInputSettings>();
		check( WidgetInputSettings );

		IMC_CommonInput = WidgetInputSettings->BuildInputMappingContext(
			[this]( FName InputName, UInputAction* IA ) { IA_CommonInputActions.Emplace( InputName, IA ); } );
		if ( IMC_CommonInput )
		{
			EnhancedInputSubsystem->AddMappingContext( IMC_CommonInput, 0 );
		}

		EnhancedInputSubsystem->AddMappingContext( WidgetInputSettings->GetOptionalInputMappingContext(), 0 );
	}
#if WITH_EDITOR
	else if ( GIsEditor )
	{
		FMessageLog AssetCheckLog( "AssetCheck" );

		FText Message( NSLOCTEXT( "EWN", "NoSupportEnhancedInput",
			"Project does not support EnhancedInput, check the DefaultPlayerInputClass in ProjectSettings." ) );
		AssetCheckLog.Error( Message );

		AssetCheckLog.Notify( Message, EMessageSeverity::Error, true );
	}
#endif
}

ETriggerEvent UEWN_WidgetInputSubsystem::GetTriggerEvent( UInputAction* IA ) const
{
	auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	if ( UEnhancedPlayerInput* EnhancedPI = EnhancedInputSubsystem->GetPlayerInput() )
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
		if ( const FEWN_InputMappingOverrides* InputMappingOverride =
				 InputMappingOverrides.Find( GetTypeHash( FObjectKey( ContextObject ) ) ) )
		{
			if ( InputMappingOverride->InputActions.Contains( InputName ) )
			{
				return InputMappingOverride->InputActions[InputName];
			}
		}
		return IA_CommonInputActions.Contains( InputName ) ? IA_CommonInputActions[InputName] : nullptr;
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

	FEWN_InputMappingOverrides& InputMappingOverride = InputMappingOverrides.Emplace( GetTypeHash( FObjectKey( ContextObject ) ) );

	InputMappingOverride.InputMappingContext = GetDefault<UEWN_WidgetInputSettings>()->BuildInputMappingContext( InjectionSettings,
		[&]( FName InputName, UInputAction* IA ) { InputMappingOverride.InputActions.Emplace( InputName, IA ); } );
	if ( InputMappingOverride.InputMappingContext )
	{
		auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check( EnhancedInputSubsystem );

		EnhancedInputSubsystem->AddMappingContext( InputMappingOverride.InputMappingContext, 0 );
	}
}

void UEWN_WidgetInputSubsystem::ClearInputMappingContext( const UObject* ContextObject )
{
	if ( FEWN_InputMappingOverrides* InputMappingOverride =
			 InputMappingOverrides.Find( GetTypeHash( FObjectKey( ContextObject ) ) ) )
	{
		auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check( EnhancedInputSubsystem );

		EnhancedInputSubsystem->RemoveMappingContext( InputMappingOverride->InputMappingContext );
	}
}

EEWN_WidgetInputMode UEWN_WidgetInputSubsystem::GetCurrentInputMode() const
{
	return CurrentMode;
}

void UEWN_WidgetInputSubsystem::SetCurrentInputMode( EEWN_WidgetInputMode NewMode )
{
	if ( CurrentMode != NewMode )
	{
		CurrentMode = NewMode;
		BroadcastInputMethodChanged();
	}
}

void UEWN_WidgetInputSubsystem::BroadcastInputMethodChanged()
{
	UWorld* World = GetWorld();
	if ( World && !World->bIsTearingDown )
	{
		OnInputMethodChangedDelegate.Broadcast( CurrentMode );
	}
}
