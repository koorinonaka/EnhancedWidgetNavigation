// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

// Interfaces
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"

#include "EWN_NavigationButton.generated.h"

UCLASS( Abstract )
class ENHANCEDWIDGETNAVIGATION_API UEWN_NavigationButton : public UUserWidget, public IEWN_Interface_WidgetNavigationChild
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual FReply NativeOnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;

	// IEWN_Interface_WidgetNavigationChild BEGIN
protected:
	virtual bool WasJustNavigationClicked_Implementation() const override { return WasJustClicked(); }
	// IEWN_Interface_WidgetNavigationChild END

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	bool WasJustClicked() const;

private:
	bool bFocused = false;
	bool bWasJustClicked = false;
	uint32 FrameNumberOnClicked = 0;
};
