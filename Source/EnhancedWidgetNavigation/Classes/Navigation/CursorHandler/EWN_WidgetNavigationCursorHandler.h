// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"

class FEWN_WidgetNavigationCursorFactory
{
public:
	static TSharedPtr<class FEWN_WidgetNavigationCursorHandler> CreateHandler( class UEWN_WidgetNavigation* Navigation );
};

class ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetNavigationCursorHandler
{
public:
	FEWN_WidgetNavigationCursorHandler( class UEWN_WidgetNavigation* Navigation );
	virtual ~FEWN_WidgetNavigationCursorHandler() {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const = 0;

	int32 GetForwardIndex( int32 CurrentIndex ) const;
	int32 GetBackwardIndex( int32 CurrentIndex ) const;

protected:
	class UPanelWidget* GetPanelWidget() const;

protected:
	bool IsLoopNavigation() const;
	bool IsDistanceBasedNavigation() const;
	bool IsWrapLines() const;

private:
	TWeakObjectPtr<class UEWN_WidgetNavigation> OuterNavigation;
};

class ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetNavigationCursorHandler_Default : public FEWN_WidgetNavigationCursorHandler
{
public:
	FEWN_WidgetNavigationCursorHandler_Default( class UEWN_WidgetNavigation* Navigation );

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override { return CurrentIndex; }
};
