// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_Interface_WidgetNavigationChild.h"

bool IEWN_Interface_WidgetNavigationChild::IsNavigationFocusable( const UObject* OuterThis )
{
	return ImplementsInterface( OuterThis ) && Execute_IsNavigationFocusable( OuterThis );
}

bool IEWN_Interface_WidgetNavigationChild::IsNavigationAcceptable( const UObject* OuterThis )
{
	return ImplementsInterface( OuterThis ) && Execute_IsNavigationAcceptable( OuterThis );
}

void IEWN_Interface_WidgetNavigationChild::SetNavigationFocusable( UObject* OuterThis, bool bEnabled )
{
	if ( ImplementsInterface( OuterThis ) )
	{
		Execute_SetNavigationFocusable( OuterThis, bEnabled );
	}
}

void IEWN_Interface_WidgetNavigationChild::SetNavigationAcceptable( UObject* OuterThis, bool bEnabled )
{
	if ( ImplementsInterface( OuterThis ) )
	{
		Execute_SetNavigationAcceptable( OuterThis, bEnabled );
	}
}

bool IEWN_Interface_WidgetNavigationChild::WasJustNavigationClicked( const UObject* OuterThis )
{
	return ImplementsInterface( OuterThis ) && Execute_WasJustNavigationClicked( OuterThis );
}
