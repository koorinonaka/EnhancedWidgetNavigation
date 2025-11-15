// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/PanelWidget.h"
#include "EWN_WidgetTypes.h"

namespace EWN::WidgetNavigation
{
struct ENHANCEDWIDGETNAVIGATION_API FWidgetWithNavigation
{
	class UEWN_WidgetNavigation* Navigation = nullptr;
	int32 Index = INDEX_NONE;
	FVector2D Position;
};

class ENHANCEDWIDGETNAVIGATION_API FHelper
{
public:
	static int32 FindPanelIndex( const UPanelWidget* PanelWidget, const TFunctionRef<bool( int32, UWidget* )>& Callback );
	static void ForEachPanelChildren( const UPanelWidget* PanelWidget, const TFunctionRef<void( int32, UWidget* )>& Callback );

	static FVector2D GetCursorPosition( const FGeometry& Geometry, EEWN_WidgetCursor WidgetCursor );

	static UWidget* FindFocusToNearest( const UWidget& CurrentWidget, EEWN_WidgetCursor WidgetCursor,
		const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation );
	static UWidget* FindFocusToOpposite( const UWidget& CurrentWidget, EEWN_WidgetCursor WidgetCursor,
		const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation );
};
}	 // namespace EWN::WidgetNavigation
