// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/PanelWidget.h"
#include "CoreMinimal.h"
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"

class UEWN_WidgetNavigation;

namespace EWN::WidgetNavigation
{
class FCursorFactory
{
public:
	static TSharedPtr<class FCursorHandler> CreateHandler( UEWN_WidgetNavigation* Navigation );
};

class ENHANCEDWIDGETNAVIGATION_API FCursorHandler
{
public:
	FCursorHandler( UEWN_WidgetNavigation* Navigation );
	virtual ~FCursorHandler() {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const = 0;

	int32 GetForwardIndex( int32 CurrentIndex ) const;
	int32 GetBackwardIndex( int32 CurrentIndex ) const;

protected:
	UPanelWidget* GetPanelWidget() const;

protected:
	bool IsLoopNavigation() const;
	bool IsDistanceBasedNavigation() const;
	bool IsWrapLines() const;

private:
	TWeakObjectPtr<UEWN_WidgetNavigation> OuterNavigation;
};

class ENHANCEDWIDGETNAVIGATION_API FCursorHandler_Default final : public FCursorHandler
{
public:
	explicit FCursorHandler_Default( UEWN_WidgetNavigation* Navigation ) : FCursorHandler( Navigation ) {}

public:
	virtual int32 GetNextIndex( int32 CurrentIndex, EEWN_WidgetCursor WidgetCursor ) const override { return CurrentIndex; }
};
}	 // namespace EWN::WidgetNavigation
