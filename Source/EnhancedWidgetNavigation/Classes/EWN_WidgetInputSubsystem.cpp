// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_WidgetInputSubsystem.h"

//
#include "Components/Widget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Framework/Application/IInputProcessor.h"

//
#include "EWN_WidgetInputSettings.h"
#include "Interfaces/EWN_Interface_LocalPlayerExtension.h"

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
		const EEWN_WidgetInputMode InputType = GetInputType( InKeyEvent.GetKey() );
		if ( IsRelevantInput( SlateApp, InKeyEvent, InputType ) )
		{
			RefreshCurrentInputMethod( InputType );
		}

		return false;
	}

	virtual bool HandleAnalogInputEvent( FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent ) override
	{
		const EEWN_WidgetInputMode InputType = GetInputType( InAnalogInputEvent.GetKey() );
		if ( IsRelevantInput( SlateApp, InAnalogInputEvent, InputType ) )
		{
			RefreshCurrentInputMethod( InputType );
		}

		return false;
	}

	virtual bool HandleMouseMoveEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		const EEWN_WidgetInputMode InputType = GetInputType( InPointerEvent );
		if ( IsRelevantInput( SlateApp, InPointerEvent, InputType ) )
		{
			if ( !InPointerEvent.GetCursorDelta().IsNearlyZero() )
			{
				RefreshCurrentInputMethod( InputType );
			}
		}

		return false;
	}

	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		const EEWN_WidgetInputMode InputType = GetInputType( InPointerEvent );
		if ( IsRelevantInput( SlateApp, InPointerEvent, InputType ) )
		{
			RefreshCurrentInputMethod( InputType );
		}

		return false;
	}

	virtual bool HandleMouseButtonDoubleClickEvent( FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent ) override
	{
		const EEWN_WidgetInputMode InputType = GetInputType( InPointerEvent );
		if ( IsRelevantInput( SlateApp, InPointerEvent, InputType ) )
		{
			RefreshCurrentInputMethod( InputType );
		}

		return false;
	}

private:
	bool IsRelevantInput( FSlateApplication& SlateApp, const FInputEvent& InputEvent, const EEWN_WidgetInputMode DesiredInputType )
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

	EEWN_WidgetInputMode GetInputType( const FKey& Key )
	{
		if ( Key.IsGamepadKey() )
		{
			return EEWN_WidgetInputMode::Gamepad;
		}

		return EEWN_WidgetInputMode::Keyboard;
	}

	EEWN_WidgetInputMode GetInputType( const FPointerEvent& PointerEvent )
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

	if ( auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() )
	{
		IMC_CommonInput = GetDefault<UEWN_WidgetInputSettings>()->BuildInputMappingContext(
			[this]( EEWN_WidgetInputType InputType, UInputAction* IA ) { IA_CommonInputActions.Emplace( InputType, IA ); } );
		if ( ensure( IMC_CommonInput ) )
		{
			EnhancedInputSubsystem->AddMappingContext( IMC_CommonInput, 0 );
		}

		EnhancedInputSubsystem->AddMappingContext( GetDefault<UEWN_WidgetInputSettings>()->GetOptionalInputMappingContext(), 0 );
	}
}

ETriggerEvent UEWN_WidgetInputSubsystem::GetTriggerEvent( UInputAction* IA ) const
{
	auto* EnhancedInputSubsystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check( EnhancedInputSubsystem );

	UEnhancedPlayerInput* EnhancedPI = EnhancedInputSubsystem->GetPlayerInput();
	check( EnhancedPI );

	if ( const FInputActionInstance* IAInstance = EnhancedPI->FindActionInstanceData( IA ) )
	{
		return IAInstance->GetTriggerEvent();
	}

	return ETriggerEvent::None;
}

ETriggerEvent UEWN_WidgetInputSubsystem::GetTriggerEvent( EEWN_WidgetInputType InputType ) const
{
	return IA_CommonInputActions.Contains( InputType ) ? GetTriggerEvent( IA_CommonInputActions[InputType] ) : ETriggerEvent::None;
}

bool UEWN_WidgetInputSubsystem::WasJustTriggered( EEWN_WidgetInputType InputType ) const
{
	return GetTriggerEvent( InputType ) == ETriggerEvent::Triggered;
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
