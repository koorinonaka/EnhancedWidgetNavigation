// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EWN_WidgetTypes.h"

class UPanelWidget;

namespace EWN::WidgetNavigationHelper
{
struct FWidgetWithNavigation
{
	class UEWN_WidgetNavigation* Navigation = nullptr;
	int32 Index = INDEX_NONE;
	FVector2D Position;
};

static int32 FindPanelIndex( UPanelWidget* PanelWidget, const TFunctionRef<bool( int32, UWidget* )> Callback );
static void ForEachPanelChildren( UPanelWidget* PanelWidget, const TFunctionRef<void( int32, UWidget* )> Callback );

static FVector2D GetCursorPosition( const FGeometry& Geometry, EEWN_WidgetCursor WidgetCursor );

static UWidget* FindFocusToNearest(
	UWidget* CurrentWidget, EEWN_WidgetCursor WidgetCursor, const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation );
static UWidget* FindFocusToFarthest(
	UWidget* CurrentWidget, EEWN_WidgetCursor WidgetCursor, const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation );
};	  // namespace EWN::WidgetNavigationHelper
