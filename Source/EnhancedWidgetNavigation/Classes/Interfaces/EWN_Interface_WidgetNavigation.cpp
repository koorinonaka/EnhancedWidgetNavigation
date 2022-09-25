// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_Interface_WidgetNavigation.h"

bool IEWN_Interface_WidgetNavigation::TryMoveFocusOverride( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	return MoveFocusOverrideDelegate.IsBound() && MoveFocusOverrideDelegate.Execute( this, WidgetCursor, bFromOperation );
}

bool IEWN_Interface_WidgetNavigation::TryMoveFocusFallback( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	return MoveFocusFallbackDelegate.IsBound() && MoveFocusFallbackDelegate.Execute( this, WidgetCursor, bFromOperation );
}
