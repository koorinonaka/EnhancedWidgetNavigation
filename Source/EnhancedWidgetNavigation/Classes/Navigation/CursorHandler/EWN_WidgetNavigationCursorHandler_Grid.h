// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "EWN_WidgetNavigationCursorHandler.h"

namespace EWN::WidgetNavigation
{
class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Grid final : public FCursorHandler
{
public:
	static bool IsGrid( UPanelWidget* PanelWidget );

public:
	explicit FCursorHandler_Grid( UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation )
	{
		InitSimple();
		InitDistanceBased();
	}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const override;

private:
	class FSimpleNav;
	TPimplPtr<FSimpleNav> SimpleNav;

	void InitSimple();
	int32 GetNextIndexFromSimple( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const;

private:
	class FDistanceBasedNav;
	TPimplPtr<FDistanceBasedNav> DistanceBasedNav;

	void InitDistanceBased();
	int32 GetNextIndexFromDistanceBased( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor, bool bLoopIgnored ) const;
};
}	 // namespace EWN::WidgetNavigation
