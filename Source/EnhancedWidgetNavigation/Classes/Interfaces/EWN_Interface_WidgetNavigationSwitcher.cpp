// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_Interface_WidgetNavigationSwitcher.h"

bool IEWN_Interface_WidgetNavigationSwitcher::TryMoveFocusOverride( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	return MoveFocusOverrideDelegate.IsBound() && MoveFocusOverrideDelegate.Execute( this, WidgetCursor, bFromOperation );
}

bool IEWN_Interface_WidgetNavigationSwitcher::TryMoveFocusFallback( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	return MoveFocusFallbackDelegate.IsBound() && MoveFocusFallbackDelegate.Execute( this, WidgetCursor, bFromOperation );
}
