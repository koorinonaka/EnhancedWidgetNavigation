// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EWN_WidgetTypes.h"
#include "InputTriggers.h"
#include "Subsystems/LocalPlayerSubsystem.h"

#include "EWN_WidgetInputSubsystem.generated.h"

class IEWN_Interface_PlayerInputExtension;

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetInputSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	friend class FEWN_WidgetInputProcessor;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FInputMethodDelegate, EEWN_WidgetInputMethod, InputMethod );

public:
	static UEWN_WidgetInputSubsystem* Get( UWidget* Widget );

protected:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	virtual void Deinitialize() override;

public:
	void InitPlayerInput( IEWN_Interface_PlayerInputExtension* IPlayerInputExtension );

public:
	UInputAction* GetInputAction( const UObject* ContextObject, FName InputName ) const;

	ETriggerEvent GetTriggerEvent( UInputAction* IA ) const;
	ETriggerEvent GetTriggerEvent( const UObject* ContextObject, FName InputName ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation", meta = ( DefaultToSelf = "ContextObject" ) )
	bool WasJustTriggered( const UObject* ContextObject, FName InputName ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation", meta = ( DefaultToSelf = "ContextObject" ) )
	void SetInputMappingContext( const UObject* ContextObject, const FEWN_WidgetInputMappingContainer& InjectionSettings );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation", meta = ( DefaultToSelf = "ContextObject" ) )
	void ClearInputMappingContext( const UObject* ContextObject );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	EEWN_WidgetInputMethod GetCurrentInputMethod() const;

private:
	void SetCurrentInputMethod( EEWN_WidgetInputMethod NewMode );
	void BroadcastInputMethodChanged();

public:
	UPROPERTY( BlueprintAssignable, Category = "User Interface|Navigation" )
	FInputMethodDelegate OnInputMethodChangedDelegate;

private:
	TSharedPtr<FEWN_WidgetInputProcessor> InputProcessor;
	EEWN_WidgetInputMethod CurrentMode;
};
