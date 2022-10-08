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
	return bWasJustClicked;
}
