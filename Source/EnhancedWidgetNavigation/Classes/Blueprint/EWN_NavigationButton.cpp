// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_NavigationButton.h"

//
#include "Interfaces/EWN_Interface_PlayerInputExtension.h"

void UEWN_NavigationButton::NativeConstruct()
{
	Super::NativeConstruct();

	if ( APlayerController* PC = GetOwningPlayer() )
	{
		if ( auto* IPlayerInputExt = Cast<IEWN_Interface_PlayerInputExtension>( PC->PlayerInput ) )
		{
			IPlayerInputExt->ProcessInputStackDelegate.AddWeakLambda(
				this, [this] { bWasJustClicked = FrameNumberOnClicked == GFrameNumber; } );
		}
	}
}

void UEWN_NavigationButton::NativeDestruct()
{
	if ( APlayerController* PC = GetOwningPlayer() )
	{
		if ( auto* IPlayerInputExt = Cast<IEWN_Interface_PlayerInputExtension>( PC->PlayerInput ) )
		{
			IPlayerInputExt->ProcessInputStackDelegate.RemoveAll( this );
		}
	}

	Super::NativeDestruct();
}

FReply UEWN_NavigationButton::NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
	// FReply::Unhandled()を呼ぶと、MouseLeaveに遷移してしまう
	return FReply::Handled().CaptureMouse( GetCachedWidget().ToSharedRef() );
}

FReply UEWN_NavigationButton::NativeOnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
	FReply Reply = FReply::Handled();

	// If the user hasn't requested a new mouse captor and the button still has mouse capture,
	// then the default behavior of the button is to release mouse capture.
	if ( Reply.GetMouseCaptor().IsValid() == false && HasMouseCapture() )
	{
		bool bEventOverButton = IsHovered();
		if ( bEventOverButton )
		{
			FrameNumberOnClicked = GFrameNumber;
		}

		Reply.ReleaseMouseCapture();
	}

	return Reply;
}

bool UEWN_NavigationButton::WasJustClicked() const
{
	return bWasJustClicked;
}
