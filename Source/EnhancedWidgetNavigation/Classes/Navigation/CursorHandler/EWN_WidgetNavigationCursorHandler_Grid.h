// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EWN_WidgetNavigationCursorHandler.h"

#include "EWN_WidgetNavigationCursorHandler_Grid.generated.h"

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationCursorHandler_Grid : public UEWN_WidgetNavigationCursorHandler
{
	GENERATED_BODY()

public:
	static bool IsGrid( class UPanelWidget* PanelWidget );

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) override;

protected:
	FIntPoint GetGridPoint( int32 Index ) const;
	void FillGridWidgets( TMap<FIntPoint, UWidget*>& OutWidgets, FIntPoint& OutLargestPoint ) const;

	void SearchWithIncrement( int32 From1, int32 Max1, int32 From2, int32 Max2, const TFunctionRef<bool( int32, int32 )> Callback );
	void SearchWithDecrement( int32 From1, int32 Max1, int32 From2, int32 Max2, const TFunctionRef<bool( int32, int32 )> Callback );
};
