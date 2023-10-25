// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_MenuWidget.h"

#include "Navigation/EWN_WidgetNavigationSubsystem.h"

void UEWN_MenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( auto* WidgetNavigationSubsystem = UEWN_WidgetNavigationSubsystem::Get( this ) )
	{
		WidgetNavigationSubsystem->MenuConstruct( this );
	}
}

void UEWN_MenuWidget::NativeDestruct()
{
	if ( auto* WidgetNavigationSubsystem = UEWN_WidgetNavigationSubsystem::Get( this ) )
	{
		WidgetNavigationSubsystem->MenuDestruct( this );
	}

	Super::NativeDestruct();
}
