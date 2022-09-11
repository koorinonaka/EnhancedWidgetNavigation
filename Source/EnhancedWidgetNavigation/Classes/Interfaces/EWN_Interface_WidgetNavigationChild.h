// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "EWN_Interface_WidgetNavigationChild.generated.h"

UINTERFACE()
class ENHANCEDWIDGETNAVIGATION_API UEWN_Interface_WidgetNavigationChild : public UInterface
{
	GENERATED_BODY()
};

class ENHANCEDWIDGETNAVIGATION_API IEWN_Interface_WidgetNavigationChild
{
	GENERATED_BODY()

public:
	static bool ImplementsInterface( const UObject* OuterThis )
	{
		return OuterThis && OuterThis->GetClass()->ImplementsInterface( UEWN_Interface_WidgetNavigationChild::StaticClass() );
	}

public:
	static bool IsNavigationFocusable( const UObject* OuterThis );
	static bool IsNavigationAcceptable( const UObject* OuterThis );

	static void SetNavigationFocusable( UObject* OuterThis, bool bEnabled );
	static void SetNavigationAcceptable( UObject* OuterThis, bool bEnabled );

	static bool WasJustNavigationClicked( const UObject* OuterThis );

protected:
	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, DisplayName = "Is Navigation Focusable" )
	bool IsNavigationFocusable() const;
	virtual bool IsNavigationFocusable_Implementation() const { return bNavigationFocusable; }

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, DisplayName = "Is Navigation Acceptable" )
	bool IsNavigationAcceptable() const;
	virtual bool IsNavigationAcceptable_Implementation() const { return bNavigationAcceptable; }

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, DisplayName = "On Navigation Focusable" )
	void SetNavigationFocusable( bool bEnabled );
	virtual void SetNavigationFocusable_Implementation( bool bEnabled ) { bNavigationFocusable = bEnabled; }

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, DisplayName = "On Navigation Acceptable" )
	void SetNavigationAcceptable( bool bEnabled );
	virtual void SetNavigationAcceptable_Implementation( bool bEnabled ) { bNavigationAcceptable = bEnabled; }

	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, DisplayName = "Was Just Navigation Clicked" )
	bool WasJustNavigationClicked() const;

	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent, DisplayName = "On Navigation Focused" )
	void K2_SetNavigationFocused( bool bFocused, bool bFromOperation );

	UFUNCTION( BlueprintCallable, BlueprintImplementableEvent, DisplayName = "On Navigation Accepted" )
	void K2_SetNavigationAccepted( bool bFromOperation );

private:
	bool bNavigationFocusable = true;
	bool bNavigationAcceptable = true;
};
