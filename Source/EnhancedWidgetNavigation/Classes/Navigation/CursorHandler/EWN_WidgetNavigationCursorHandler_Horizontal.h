// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

#include "EWN_WidgetNavigationCursorHandler_Horizontal.generated.h"

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationCursorHandler_Horizontal : public UEWN_WidgetNavigationCursorHandler
{
	GENERATED_BODY()

public:
	static bool IsHorizontal( class UPanelWidget* PanelWidget );

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) override;
};
