// Fill out your copyright notice in the Description page of Project Settings.

#include "EWN_NavigationButton.h"

FReply UEWN_NavigationButton::NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
	bPressed = true;
	return FReply::Handled();	 // FReply::Unhandled()を呼ぶと、MouseLeaveに遷移してしまう
}

FReply UEWN_NavigationButton::NativeOnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
	if ( bPressed )
	{
		bool bEventOverButton = IsHovered();
		if ( bEventOverButton )
		{
			FrameNumberOnClicked = GFrameNumber;
		}

		bPressed = false;
	}

	return FReply::Handled();
}

bool UEWN_NavigationButton::WasJustClicked() const
{
	return FrameNumberOnClicked >= ( GFrameNumber - 1 );
}
