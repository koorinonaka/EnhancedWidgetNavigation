// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "EWN_WidgetTypes.h"

#include "EWN_Interface_WidgetNavigation.generated.h"

UINTERFACE( BlueprintType, meta = ( CannotImplementInterfaceInBlueprint ) )
class ENHANCEDWIDGETNAVIGATION_API UEWN_Interface_WidgetNavigation : public UInterface
{
	GENERATED_BODY()
};

class ENHANCEDWIDGETNAVIGATION_API IEWN_Interface_WidgetNavigation
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
	void SetMoveFocusOverride( const FMoveFocusDelegate& Delegate ) { MoveFocusOverrideDelegate = Delegate; }
	void SetMoveFocusFallback( const FMoveFocusDelegate& Delegate ) { MoveFocusFallbackDelegate = Delegate; }

protected:
	bool TryMoveFocusOverride( EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool TryMoveFocusFallback( EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

private:
	FMoveFocusDelegate MoveFocusOverrideDelegate;
	FMoveFocusDelegate MoveFocusFallbackDelegate;

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	virtual EEWN_WidgetInputType TickNavigation( float DeltaTime )
		PURE_VIRTUAL( ThisClass::TickNavigation, return EEWN_WidgetInputType::None; );

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	virtual bool IsNavigationEnabled() const { return bNavigationEnabled; }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	virtual void SetNavigationEnabled( bool bEnabled ) { bNavigationEnabled = bEnabled; }

private:
	bool bNavigationEnabled = true;
};
