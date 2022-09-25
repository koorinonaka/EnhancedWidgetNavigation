// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

//
#include "InputTriggers.h"

//
#include "EWN_WidgetTypes.h"

#include "EWN_WidgetInputSubsystem.generated.h"

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
	ETriggerEvent GetTriggerEvent( EEWN_WidgetInputType InputType ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	bool WasJustTriggered( EEWN_WidgetInputType InputType ) const;

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
	TMap<EEWN_WidgetInputType, TObjectPtr<class UInputAction>> IA_CommonInputActions;
};
