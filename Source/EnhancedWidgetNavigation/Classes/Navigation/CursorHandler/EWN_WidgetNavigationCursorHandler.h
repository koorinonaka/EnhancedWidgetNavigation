// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"

#include "EWN_WidgetNavigationCursorHandler.generated.h"

class FEWN_WidgetNavigationCursorFactory
{
public:
	static UEWN_WidgetNavigationCursorHandler* CreateHandler( class UEWN_WidgetNavigation* Navigation );
};

UCLASS( Abstract )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationCursorHandler : public UObject
{
	GENERATED_BODY()

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor )
		PURE_VIRTUAL( ThisClass::GetNextIndex, return INDEX_NONE; );

	int32 GetForwardIndex( int32 CurrentIndex ) const;
	int32 GetBackwardIndex( int32 CurrentIndex ) const;

protected:
	class UPanelWidget* GetPanelWidget() const;

protected:
	bool IsLoopNavigation() const;
	bool IsWrapLines() const;
};

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationCursorHandler_Default : public UEWN_WidgetNavigationCursorHandler
{
	GENERATED_BODY()

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) override { return CurrentIndex; }
};
