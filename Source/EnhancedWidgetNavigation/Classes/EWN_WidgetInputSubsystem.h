// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

//
#include "InputTriggers.h"

//
#include "EWN_WidgetTypes.h"

#include "EWN_WidgetInputSubsystem.generated.h"

USTRUCT()
struct ENHANCEDWIDGETNAVIGATION_API FEWN_InputMappingOverrides
{
	GENERATED_BODY()

	UPROPERTY( Transient )
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY( Transient )
	TMap<FName, TObjectPtr<class UInputAction>> InputActions;
};

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetInputSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	friend class FEWN_WidgetInputProcessor;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FInputModeDelegate, EEWN_WidgetInputMode, InputMode );

public:
	static UEWN_WidgetInputSubsystem* Get( UWidget* Widget );

protected:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	virtual void Deinitialize() override;

	virtual void InitOnSpawnPlayActor( APlayerController* PlayActor );

public:
	ETriggerEvent GetTriggerEvent( class UInputAction* IA ) const;
	ETriggerEvent GetTriggerEvent( const UObject* ContextObject, FName InputName ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation", meta = ( DefaultToSelf = "ContextObject" ) )
	bool WasJustTriggered( const UObject* ContextObject, FName InputName ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation", meta = ( DefaultToSelf = "ContextObject" ) )
	void SetInputMappingContext( const UObject* ContextObject, const FEWN_WidgetInputMappingContainer& InjectionSettings );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation", meta = ( DefaultToSelf = "ContextObject" ) )
	void ClearInputMappingContext( const UObject* ContextObject );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	EEWN_WidgetInputMode GetCurrentInputMode() const;

private:
	void SetCurrentInputMode( EEWN_WidgetInputMode NewMode );
	void BroadcastInputMethodChanged();

public:
	UPROPERTY( BlueprintAssignable, Category = "User Interface|Navigation" )
	FInputModeDelegate OnInputMethodChangedDelegate;

private:
	TSharedPtr<class FEWN_WidgetInputProcessor> InputProcessor;
	EEWN_WidgetInputMode CurrentMode;

	UPROPERTY( Transient )
	TObjectPtr<class UInputMappingContext> IMC_CommonInput;

	UPROPERTY( Transient )
	TMap<FName, TObjectPtr<class UInputAction>> IA_CommonInputActions;

	UPROPERTY( Transient )
	TMap<uint32, FEWN_InputMappingOverrides> InputMappingOverrides;
};
