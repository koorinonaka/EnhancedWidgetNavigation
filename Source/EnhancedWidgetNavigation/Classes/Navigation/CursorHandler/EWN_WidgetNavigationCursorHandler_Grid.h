// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

//
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

class ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetNavigationCursorHandler_Grid : public FEWN_WidgetNavigationCursorHandler
{
public:
	static bool IsGrid( class UPanelWidget* PanelWidget );

public:
	FEWN_WidgetNavigationCursorHandler_Grid( class UEWN_WidgetNavigation* Navigation );

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
