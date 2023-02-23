// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_EnhancedInputComponent.h"

//
#include "Engine/LocalPlayer.h"

//
#include "EWN_WidgetInputSubsystem.h"
#include "Interfaces/EWN_Interface_PlayerInputExtension.h"

void UEWN_EnhancedInputComponent::OnRegister()
{
	Super::OnRegister();

	if ( auto* OwnerPC = GetOwner<APlayerController>() )
	{
		if ( auto* LocalPlayer = Cast<ULocalPlayer>( OwnerPC->Player ) )
		{
			if ( auto* WidgetInputSubsystem = LocalPlayer->GetSubsystem<UEWN_WidgetInputSubsystem>() )
			{
				WidgetInputSubsystem->InitPlayerInput( Cast<IEWN_Interface_PlayerInputExtension>( OwnerPC->PlayerInput ) );
			}
		}
	}
}
