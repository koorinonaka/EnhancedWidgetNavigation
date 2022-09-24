// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_Interface_WidgetNavigationConnection.h"

bool IEWN_Interface_WidgetNavigationConnection::TryMoveFocusOverride( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	return MoveFocusOverrideDelegate.IsBound() && MoveFocusOverrideDelegate.Execute( this, WidgetCursor, bFromOperation );
}

bool IEWN_Interface_WidgetNavigationConnection::TryMoveFocusFallback( EEWN_WidgetCursor WidgetCursor, bool bFromOperation )
{
	return MoveFocusFallbackDelegate.IsBound() && MoveFocusFallbackDelegate.Execute( this, WidgetCursor, bFromOperation );
}
