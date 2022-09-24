// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

//
#include "EWN_WidgetTypes.h"

#include "EWN_Interface_WidgetNavigationSwitcher.generated.h"

UINTERFACE( BlueprintType, meta = ( CannotImplementInterfaceInBlueprint ) )
class ENHANCEDWIDGETNAVIGATION_API UEWN_Interface_WidgetNavigationSwitcher : public UInterface
{
	GENERATED_BODY()
};

class ENHANCEDWIDGETNAVIGATION_API IEWN_Interface_WidgetNavigationSwitcher
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_RetVal_ThreeParams(
		bool, FMoveFocusDelegate, ThisClass* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

	DECLARE_MULTICAST_DELEGATE_FourParams(
		FFocusDelegate, ThisClass* INavigation, int32 OldIndex, int32 NewIndex, bool bFromOperation );
	FFocusDelegate NavigationUpdatedDelegate;

public:
	virtual UWidget* GetCurrentWidget() const = 0;
	virtual bool TestFocus( EEWN_WidgetCursor WidgetCursor ) const = 0;
	virtual void ForEachWidgetNavigation( const TFunctionRef<void( class UEWN_WidgetNavigation* )> Callback ) = 0;

	virtual void InvalidateNavigation() = 0;

public:
	void SetMoveFocusOverride( FMoveFocusDelegate Delegate ) { MoveFocusOverrideDelegate = Delegate; }
	void SetMoveFocusFallback( FMoveFocusDelegate Delegate ) { MoveFocusFallbackDelegate = Delegate; }

protected:
	bool TryMoveFocusOverride( EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool TryMoveFocusFallback( EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

private:
	FMoveFocusDelegate MoveFocusOverrideDelegate;
	FMoveFocusDelegate MoveFocusFallbackDelegate;

public:
	UFUNCTION( BlueprintCallable )
	virtual EEWN_WidgetInputType TickNavigation( float DeltaTime )
		PURE_VIRTUAL( ThisClass::TickNavigation, return EEWN_WidgetInputType::None; );

public:
	UFUNCTION( BlueprintCallable )
	virtual bool IsNavigationEnabled() const { return bNavigationEnabled; }

	UFUNCTION( BlueprintCallable )
	virtual void SetNavigationEnabled( bool bEnabled ) { bNavigationEnabled = bEnabled; }

private:
	bool bNavigationEnabled = true;
};
