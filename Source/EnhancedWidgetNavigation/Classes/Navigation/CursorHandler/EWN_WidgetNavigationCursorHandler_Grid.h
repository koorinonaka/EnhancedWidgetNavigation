// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

//
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

namespace EWN::WidgetNavigation
{
class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Grid : public FCursorHandler
{
public:
	static bool IsGrid( class UPanelWidget* PanelWidget );

public:
	FCursorHandler_Grid( class UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation )
	{
		InitSimple();
		InitDistanceBased();
	}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override;

private:
	class FSimpleNav;
	TPimplPtr<FSimpleNav> SimpleNav;

	void InitSimple();
	int32 GetNextIndexFromSimple( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const;

private:
	class FDistanceBasedNav;
	TPimplPtr<FDistanceBasedNav> DistanceBasedNav;

	void InitDistanceBased();
	int32 GetNextIndexFromDistanceBased( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const;
};
}	 // namespace EWN::WidgetNavigation
